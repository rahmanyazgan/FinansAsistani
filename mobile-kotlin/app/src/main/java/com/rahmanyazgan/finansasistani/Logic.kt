package com.rahmanyazgan.finansasistani

import org.json.JSONObject
import java.net.HttpURLConnection
import java.net.URL
import java.util.Calendar
import kotlin.math.abs
import kotlin.math.min
import kotlin.math.max
import kotlin.math.pow
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

val CURRENT_YEAR = Calendar.getInstance().get(Calendar.YEAR)

data class TaxResult(
    val yil: Int,
    val toplamPrim: Double,
    val vergiIadesi: Double,
    val matrahDusulen: Double,
    val brutAylik: Double,
    val aylikIadeListesi: List<Pair<Double, Double>>,
    val ayIsimleri: List<String>,
    val sigortaTuru: String,
    val vergiDilimi: String,
    val sonHesaplama: String
)

val UCRET_DILIMLERI = listOf(
    190000.0 to 0.15,
    400000.0 to 0.20,
    1500000.0 to 0.27,
    5300000.0 to 0.35,
    Double.MAX_VALUE to 0.40
)

fun hesaplaVergi(yillikBrut: Double, dilimler: List<Pair<Double, Double>>): Double {
    var kalan = yillikBrut
    var toplamVergi = 0.0
    var altSinir = 0.0
    for ((ustSinir, oran) in dilimler) {
        if (kalan <= 0) break
        val dilimGenislik = ustSinir - altSinir
        val vergilenecek = min(kalan, dilimGenislik)
        toplamVergi += vergilenecek * oran
        kalan -= vergilenecek
        altSinir = ustSinir
    }
    return toplamVergi
}

fun marjinalVergiOrani(yillikBrut: Double, dilimler: List<Pair<Double, Double>>): Double {
    for ((ust, oran) in dilimler) {
        if (yillikBrut <= ust) return oran
    }
    return dilimler.last().second
}

fun runCalculation(brutAylik: Double, primAylik: Double, isHayat: Boolean): TaxResult {
    val asgariUcretYillik = 396360.0
    val brutYillik = brutAylik * 12
    val sgkOrani = 0.15
    val aylikMatrah = brutAylik * (1 - sgkOrani)
    val yillikMatrah = aylikMatrah * 12

    val vergiOncesi = hesaplaVergi(yillikMatrah, UCRET_DILIMLERI)
    val indirimOrani = if (isHayat) 0.5 else 1.0
    val aylikIndirim = primAylik * indirimOrani
    val toplamIndirilebilirYillik = aylikIndirim * 12
    val ustSinirGelir = brutYillik * 0.15
    val indirimUygulananYillik = min(toplamIndirilebilirYillik, min(ustSinirGelir, asgariUcretYillik))

    val matrahSonrasi = max(yillikMatrah - indirimUygulananYillik, 0.0)
    val vergiSonrasi = hesaplaVergi(matrahSonrasi, UCRET_DILIMLERI)
    val vergiIadesi = vergiOncesi - vergiSonrasi

    val aylikIadeListesi = mutableListOf<Pair<Double, Double>>()
    val aylar = listOf("Oca", "Şub", "Mar", "Nis", "May", "Haz", "Tem", "Ağu", "Eyl", "Eki", "Kas", "Ara")
    var kalanLimit = indirimUygulananYillik
    var kumulatifMatrah = 0.0

    for (i in 0..11) {
        val uygAylik = min(aylikIndirim, kalanLimit)
        kalanLimit -= uygAylik
        kumulatifMatrah += aylikMatrah
        val oran = marjinalVergiOrani(kumulatifMatrah, UCRET_DILIMLERI)
        aylikIadeListesi.add(uygAylik * oran to oran)
    }

    return TaxResult(
        yil = CURRENT_YEAR,
        toplamPrim = primAylik * 12,
        vergiIadesi = vergiIadesi,
        matrahDusulen = indirimUygulananYillik,
        brutAylik = brutAylik,
        aylikIadeListesi = aylikIadeListesi,
        ayIsimleri = aylar,
        sigortaTuru = if (isHayat) "Hayat/Birikimli" else "Sağlık/Vefat",
        vergiDilimi = "%${(marjinalVergiOrani(yillikMatrah, UCRET_DILIMLERI) * 100).toInt()}",
        sonHesaplama = "Bugün"
    )
}

val COMMON_CURRENCIES = mapOf(
    "TRY" to "Türk Lirası",
    "USD" to "ABD Doları",
    "EUR" to "Euro",
    "GBP" to "İngiliz Sterlini",
    "JPY" to "Japon Yeni",
    "CAD" to "Kanada Doları",
    "CHF" to "İsviçre Frangı",
    "AUD" to "Avustralya Doları"
)

suspend fun fetchExchangeRates(base: String = "USD"): Map<String, Double>? = withContext(Dispatchers.IO) {
    try {
        val url = URL("https://open.er-api.com/v6/latest/$base")
        val conn = url.openConnection() as HttpURLConnection
        conn.requestMethod = "GET"
        conn.connectTimeout = 15000
        conn.readTimeout = 15000
        conn.setRequestProperty("User-Agent", "FinansAsistani/1.0")
        conn.setRequestProperty("Accept", "application/json")
        if (conn.responseCode == 200) {
            val response = conn.inputStream.bufferedReader().use { it.readText() }
            val json = JSONObject(response)
            if (json.optString("result") == "success") {
                val ratesJson = json.getJSONObject("rates")
                val ratesMap = mutableMapOf<String, Double>()
                ratesJson.keys().forEach { key ->
                    ratesMap[key] = ratesJson.getDouble(key)
                }
                return@withContext ratesMap
            }
        }
    } catch (e: Exception) {
        e.printStackTrace()
    }
    null
}

val FREQ_MAP = mapOf(
    "Günlük" to 365,
    "Haftalık" to 52,
    "Aylık" to 12,
    "3 Aylık" to 4,
    "6 Aylık" to 2,
    "Yıllık" to 1
)

data class YearRow(
    val year: Int,
    val balance: Double,
    val totalContrib: Double,
    val profit: Double
)

fun compoundFV(
    principal: Double,
    monthly: Double,
    years: Int,
    annualRate: Double,
    n: Int
): List<YearRow> {
    val r = annualRate / 100.0
    val rMo = if (r > 0) (1.0 + r / n).pow(n / 12.0) - 1.0 else 0.0
    var balance = principal
    var cum = principal
    val out = mutableListOf<YearRow>()
    for (yr in 1..years) {
        for (m in 0..11) {
            balance += monthly
            cum += monthly
            balance *= (1.0 + rMo)
        }
        out.add(YearRow(yr, balance, cum, balance - cum))
    }
    return out
}

val PERCENT_MODES = listOf(
    "Bir sayının %X'i kaçtır?",
    "X, Y'nin yüzde kaçıdır?",
    "Yüzde değişim (Artış/Azalış)",
    "KDV Hesaplama (+)",
    "KDV Hesaplama (-)"
)

fun calculatePercentage(modeIdx: Int, valA: Double, valB: Double): Double {
    return when (modeIdx) {
        0 -> (valA * valB) / 100.0
        1 -> (valA / valB) * 100.0
        2 -> ((valB - valA) / abs(valA)) * 100.0
        3 -> valA * (1.0 + valB / 100.0)
        4 -> valA / (1.0 + valB / 100.0)
        else -> 0.0
    }
}
