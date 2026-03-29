/**
 * FinansAsistani Mobil - Hesaplama Mantığı
 */

export const THEME = {
    bg_main: "#050b14",
    bg_secondary: "#0d1b2a",
    bg_input: "#1b263b",
    border: "#1e3a5f",
    accent: "#00d4ff",
    accent_hover: "#33e0ff",
    text: "#ffffff",
    text_muted: "#a9d6e5",
    result: "#00f5d4",
};

export const CURRENT_YEAR = new Date().getFullYear();

// --- TAX LOGIC ---
export type TaxResult = {
    yil: number;
    toplam_prim: number;
    vergi_iadesi: number;
    matrah_dusulen: number;
    brut_aylik: number;
    aylik_iade_listesi: { iade: number; oran: number }[];
    ay_isimleri: string[];
    sigorta_turu: string;
    vergi_dilimi: string;
    son_hesaplama: string;
};

export function hesaplaVergi(yillikBrut: number, dilimler: [number, number][]) {
    let kalan = yillikBrut;
    let toplamVergi = 0;
    let altSinir = 0;
    const detaylar = [];

    for (const [ustSinir, oran] of dilimler) {
        if (kalan <= 0) break;
        const dilimGenislik = ustSinir - altSinir;
        const vergilenecek = Math.min(kalan, dilimGenislik);
        const dilimVergi = vergilenecek * oran;
        toplamVergi += dilimVergi;
        detaylar.push({
            alt: altSinir,
            ust: Math.min(ustSinir, yillikBrut),
            oran,
            matrah: vergilenecek,
            vergi: dilimVergi,
        });
        kalan -= vergilenecek;
        altSinir = ustSinir;
    }
    return { toplamVergi, detaylar };
}

export function marjinalVergiOrani(yillikBrut: number, dilimler: [number, number][]) {
    for (const [ust, oran] of dilimler) {
        if (yillikBrut <= ust) return oran;
    }
    return dilimler[dilimler.length - 1][1];
}

const UCRET_DILIMLERI: [number, number][] = [
    [190000, 0.15],
    [400000, 0.20],
    [1500000, 0.27],
    [5300000, 0.35],
    [Number.MAX_SAFE_INTEGER, 0.4],
];

export function runCalculation(brutAylik: number, primAylik: number, isHayat: boolean): TaxResult {
    const asgariUcretYillik = 396360;
    const brutYillik = brutAylik * 12;
    const sgkOrani = 0.15;
    const aylikMatrah = brutAylik * (1 - sgkOrani);
    const yillikMatrah = aylikMatrah * 12;

    const { toplamVergi: vergiOncesi } = hesaplaVergi(yillikMatrah, UCRET_DILIMLERI);
    const indirimOrani = isHayat ? 0.5 : 1.0;
    const aylikIndirim = primAylik * indirimOrani;
    const toplamIndirilebilirYillik = aylikIndirim * 12;
    const ustSinirGelir = brutYillik * 0.15;
    const indirimUygulananYillik = Math.min(toplamIndirilebilirYillik, ustSinirGelir, asgariUcretYillik);

    const matrahSonrasi = Math.max(yillikMatrah - indirimUygulananYillik, 0);
    const { toplamVergi: vergiSonrasi } = hesaplaVergi(matrahSonrasi, UCRET_DILIMLERI);
    const vergiIadesi = vergiOncesi - vergiSonrasi;

    const aylikIadeListesi = [];
    const aylar = ["Oca", "Şub", "Mar", "Nis", "May", "Haz", "Tem", "Ağu", "Eyl", "Eki", "Kas", "Ara"];
    let kalanLimit = indirimUygulananYillik;
    let kumulatifMatrah = 0;

    for (let i = 0; i < 12; i++) {
        const uygAylik = Math.min(aylikIndirim, kalanLimit);
        kalanLimit -= uygAylik;
        kumulatifMatrah += aylikMatrah;
        const oran = marjinalVergiOrani(kumulatifMatrah, UCRET_DILIMLERI);
        aylikIadeListesi.push({ iade: uygAylik * oran, oran });
    }

    return {
        yil: CURRENT_YEAR,
        toplam_prim: primAylik * 12,
        vergi_iadesi: vergiIadesi,
        matrah_dusulen: indirimUygulananYillik,
        brut_aylik: brutAylik,
        aylik_iade_listesi: aylikIadeListesi,
        ay_isimleri: aylar,
        sigorta_turu: isHayat ? "Hayat/Birikimli" : "Sağlık/Vefat",
        vergi_dilimi: `%${(marjinalVergiOrani(yillikMatrah, UCRET_DILIMLERI) * 100).toFixed(0)}`,
        son_hesaplama: new Date().toLocaleDateString("tr-TR"),
    };
}

// --- CURRENCY LOGIC ---
export async function fetchExchangeRates(base: string = 'USD') {
    try {
        const response = await fetch(`https://open.er-api.com/v6/latest/${base}`);
        const data = await response.json();
        if (data.result === 'success') {
            return data.rates;
        }
    } catch (error) {
        console.error("Exchange rate fetch error:", error);
    }
    return null;
}

export const COMMON_CURRENCIES: { [key: string]: string } = {
    "TRY": "Türk Lirası",
    "USD": "ABD Doları",
    "EUR": "Euro",
    "GBP": "İngiliz Sterlini",
    "JPY": "Japon Yeni",
    "CAD": "Kanada Doları",
    "CHF": "İsviçre Frangı",
    "AUD": "Avustralya Doları"
};

// --- COMPOUND INTEREST LOGIC ---

export const FREQ_MAP: { [key: string]: number } = {
    "Günlük": 365,
    "Haftalık": 52,
    "Aylık": 12,
    "3 Aylık": 4,
    "6 Aylık": 2,
    "Yıllık": 1,
};

export type YearRow = {
    year: number;
    balance: number;
    totalContrib: number;
    profit: number;
};

export function compoundFV(
    principal: number,
    monthly: number,
    years: number,
    annualRate: number,
    n: number
): YearRow[] {
    const r = annualRate / 100;
    const rMo = r > 0 ? Math.pow(1 + r / n, n / 12) - 1 : 0;
    let balance = principal;
    let cum = principal;
    const out: YearRow[] = [];
    for (let yr = 1; yr <= years; yr++) {
        for (let m = 0; m < 12; m++) {
            balance += monthly;
            cum += monthly;
            balance *= (1 + rMo);
        }
        out.push({ year: yr, balance, totalContrib: cum, profit: balance - cum });
    }
    return out;
}

export const PERCENT_MODES = [
    "Bir sayının %X'i kaçtır?",
    "X, Y'nin yüzde kaçıdır?",
    "Yüzde değişim (Artış/Azalış)",
    "KDV Hesaplama (+)",
    "KDV Hesaplama (-)"
];

export function calculatePercentage(modeIdx: number, valA: number, valB: number) {
    switch (modeIdx) {
        case 0: return (valA * valB) / 100;
        case 1: return (valA / valB) * 100;
        case 2: return ((valB - valA) / Math.abs(valA)) * 100;
        case 3: return valA * (1 + valB / 100);
        case 4: return valA / (1 + valB / 100);
        default: return 0;
    }
}
