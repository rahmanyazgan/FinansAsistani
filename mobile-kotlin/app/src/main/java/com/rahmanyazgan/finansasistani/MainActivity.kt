package com.rahmanyazgan.finansasistani

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.clickable
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.*
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.window.Dialog
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.currentBackStackEntryAsState
import androidx.navigation.compose.rememberNavController
import kotlinx.coroutines.launch
import java.text.NumberFormat
import java.util.Calendar
import java.util.Locale

val bg_main = Color(0xFF050B14)
val bg_secondary = Color(0xFF0D1B2A)
val bg_input = Color(0xFF1B263B)
val border = Color(0xFF1E3A5F)
val accent = Color(0xFF00D4FF)
val text_main = Color.White
val text_muted = Color(0xFFA9D6E5)
val result_color = Color(0xFF00F5D4)

val trLocale = Locale("tr", "TR")
val trNumberFormat: NumberFormat = NumberFormat.getIntegerInstance(trLocale)

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            MaterialTheme {
                AppScreen()
            }
        }
    }
}

@Composable
fun AppScreen() {
    val navController = rememberNavController()
    Scaffold(
        bottomBar = {
            NavigationBar(
                containerColor = bg_secondary,
                contentColor = accent,
                tonalElevation = 0.dp
            ) {
                val navBackStackEntry by navController.currentBackStackEntryAsState()
                val currentRoute = navBackStackEntry?.destination?.route

                NavigationBarItem(
                    selected = currentRoute == "Kur",
                    onClick = { navController.navigate("Kur") },
                    icon = { Icon(Icons.Default.Refresh, contentDescription = null) },
                    label = { Text("Kur") },
                    colors = NavigationBarItemDefaults.colors(
                        selectedIconColor = accent, unselectedIconColor = text_muted,
                        selectedTextColor = accent, unselectedTextColor = text_muted,
                        indicatorColor = bg_input
                    )
                )
                NavigationBarItem(
                    selected = currentRoute == "Yuzde",
                    onClick = { navController.navigate("Yuzde") },
                    icon = { Icon(Icons.Default.Percent, contentDescription = null) },
                    label = { Text("Yüzde") },
                    colors = NavigationBarItemDefaults.colors(
                        selectedIconColor = accent, unselectedIconColor = text_muted,
                        selectedTextColor = accent, unselectedTextColor = text_muted,
                        indicatorColor = bg_input
                    )
                )
                NavigationBarItem(
                    selected = currentRoute == "Vergi",
                    onClick = { navController.navigate("Vergi") },
                    icon = { Icon(Icons.Default.Calculate, contentDescription = null) },
                    label = { Text("Vergi") },
                    colors = NavigationBarItemDefaults.colors(
                        selectedIconColor = accent, unselectedIconColor = text_muted,
                        selectedTextColor = accent, unselectedTextColor = text_muted,
                        indicatorColor = bg_input
                    )
                )
                NavigationBarItem(
                    selected = currentRoute == "Kar",
                    onClick = { navController.navigate("Kar") },
                    icon = { Icon(Icons.Default.TrendingUp, contentDescription = null) },
                    label = { Text("Kar") },
                    colors = NavigationBarItemDefaults.colors(
                        selectedIconColor = accent, unselectedIconColor = text_muted,
                        selectedTextColor = accent, unselectedTextColor = text_muted,
                        indicatorColor = bg_input
                    )
                )
            }
        }
    ) { innerPadding ->
        NavHost(navController = navController, startDestination = "Kur", modifier = Modifier.padding(innerPadding)) {
            composable("Kur") { CurrencyScreen() }
            composable("Yuzde") { PercentScreen() }
            composable("Vergi") { TaxScreen() }
            composable("Kar") { CompoundScreen() }
        }
    }
}

// --- SUMMARY CARD ---
@Composable
fun SummaryCard(
    title: String,
    value: String,
    icon: @Composable () -> Unit,
    modifier: Modifier = Modifier
) {
    Column(
        modifier = modifier
            .clip(RoundedCornerShape(12.dp))
            .background(bg_secondary)
            .border(1.dp, border, RoundedCornerShape(12.dp))
            .padding(10.dp)
    ) {
        Row(
            verticalAlignment = Alignment.CenterVertically,
            modifier = Modifier.padding(bottom = 5.dp)
        ) {
            icon()
            Spacer(modifier = Modifier.width(4.dp))
            Text(
                title.uppercase(),
                color = text_muted,
                fontSize = 8.sp,
                fontWeight = FontWeight.Bold,
                maxLines = 1
            )
        }
        Text(
            value,
            color = text_main,
            fontSize = 12.sp,
            fontWeight = FontWeight.Bold,
            maxLines = 1
        )
    }
}

// --- CURRENCY SCREEN ---
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun CurrencyScreen() {
    var brutAylik by remember { mutableStateOf("100") }
    var fromCurr by remember { mutableStateOf("USD") }
    var toCurr by remember { mutableStateOf("TRY") }
    var result by remember { mutableStateOf<String?>(null) }
    var lastAmount by remember { mutableStateOf<String?>(null) }
    var rates by remember { mutableStateOf<Map<String, Double>?>(null) }
    var isLoading by remember { mutableStateOf(false) }
    var showFromPicker by remember { mutableStateOf(false) }
    var showToPicker by remember { mutableStateOf(false) }
    val scope = rememberCoroutineScope()

    fun loadRates() {
        scope.launch {
            isLoading = true
            val res = fetchExchangeRates(fromCurr)
            rates = res
            isLoading = false
            if (res != null && res.containsKey(toCurr)) {
                val amount = brutAylik.toDoubleOrNull() ?: 0.0
                result = String.format(trLocale, "%.2f", amount * res[toCurr]!!)
                lastAmount = amount.toString()
            }
        }
    }

    LaunchedEffect(fromCurr) {
        loadRates()
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(bg_main)
            .verticalScroll(rememberScrollState())
            .padding(20.dp)
    ) {
        Text("Döviz Dönüştürücü", fontSize = 24.sp, fontWeight = FontWeight.Bold, color = text_main)
        Text("Canlı Kurlar 💱", fontSize = 14.sp, color = text_muted, modifier = Modifier.padding(top = 4.dp, bottom = 20.dp))

        Column(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(15.dp))
                .background(bg_secondary)
                .border(1.dp, border, RoundedCornerShape(15.dp))
                .padding(15.dp)
        ) {
            Text("Miktar", fontSize = 11.sp, fontWeight = FontWeight.Bold, color = text_muted, modifier = Modifier.padding(bottom = 5.dp))
            OutlinedTextField(
                value = brutAylik,
                onValueChange = { brutAylik = it; result = null },
                modifier = Modifier.fillMaxWidth().padding(bottom = 15.dp),
                keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                colors = TextFieldDefaults.outlinedTextFieldColors(
                    containerColor = bg_input,
                    focusedTextColor = text_main,
                    unfocusedTextColor = text_main,
                    focusedBorderColor = border,
                    unfocusedBorderColor = border
                )
            )

            Row(
                verticalAlignment = Alignment.CenterVertically,
                modifier = Modifier.padding(bottom = 15.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                Box(
                    modifier = Modifier
                        .weight(1f)
                        .clip(RoundedCornerShape(8.dp))
                        .background(bg_input)
                        .clickable { showFromPicker = true }
                        .padding(12.dp),
                    contentAlignment = Alignment.Center
                ) {
                    Text(fromCurr, color = text_main, fontWeight = FontWeight.Bold, fontSize = 16.sp)
                }

                IconButton(
                    onClick = {
                        val temp = fromCurr
                        fromCurr = toCurr
                        toCurr = temp
                        result = null
                        lastAmount = null
                    },
                    modifier = Modifier.padding(horizontal = 4.dp)
                ) {
                    Icon(Icons.Default.Refresh, contentDescription = "Değiştir", tint = accent)
                }

                Box(
                    modifier = Modifier
                        .weight(1f)
                        .clip(RoundedCornerShape(8.dp))
                        .background(bg_input)
                        .clickable { showToPicker = true }
                        .padding(12.dp),
                    contentAlignment = Alignment.Center
                ) {
                    Text(toCurr, color = text_main, fontWeight = FontWeight.Bold, fontSize = 16.sp)
                }
            }

            Button(
                onClick = {
                    if (rates?.containsKey(toCurr) == true) {
                        val amount = brutAylik.toDoubleOrNull() ?: 0.0
                        result = String.format(trLocale, "%.2f", amount * rates!![toCurr]!!)
                        lastAmount = amount.toString()
                    } else {
                        loadRates()
                    }
                },
                modifier = Modifier.fillMaxWidth().height(50.dp),
                colors = ButtonDefaults.buttonColors(containerColor = accent),
                enabled = !isLoading
            ) {
                if (isLoading) {
                    CircularProgressIndicator(color = text_main, modifier = Modifier.size(24.dp), strokeWidth = 2.dp)
                } else {
                    Text("Dönüştür", color = text_main, fontWeight = FontWeight.Bold, fontSize = 16.sp)
                }
            }
        }

        if (result != null && lastAmount != null) {
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(vertical = 20.dp)
                    .clip(RoundedCornerShape(12.dp))
                    .background(bg_input)
                    .border(width = 4.dp, color = accent)
                    .padding(15.dp),
                contentAlignment = Alignment.Center
            ) {
                Text("$lastAmount $fromCurr = $result $toCurr", color = result_color, fontSize = 20.sp, fontWeight = FontWeight.Bold)
            }
        }

        Spacer(modifier = Modifier.height(20.dp))

        Column(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(15.dp))
                .background(bg_secondary)
                .border(1.dp, border, RoundedCornerShape(15.dp))
                .padding(15.dp)
        ) {
            Row(
                modifier = Modifier.fillMaxWidth().padding(bottom = 15.dp),
                horizontalArrangement = Arrangement.SpaceBetween,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text("Popüler Kurlar (1 $fromCurr)", color = text_main, fontSize = 18.sp, fontWeight = FontWeight.Bold)
                if (isLoading) {
                    CircularProgressIndicator(color = accent, modifier = Modifier.size(16.dp), strokeWidth = 2.dp)
                }
            }

            rates?.let { r ->
                COMMON_CURRENCIES.keys.filter { it != fromCurr && r.containsKey(it) }.forEach { code ->
                    Row(
                        modifier = Modifier
                            .fillMaxWidth()
                            .clickable {
                                toCurr = code
                                if (rates?.containsKey(code) == true) {
                                    val amount = brutAylik.toDoubleOrNull() ?: 0.0
                                    result = String.format(trLocale, "%.2f", amount * rates!![code]!!)
                                    lastAmount = amount.toString()
                                }
                            }
                            .padding(vertical = 12.dp),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Column {
                            Text(code, color = text_main, fontSize = 14.sp, fontWeight = FontWeight.Bold)
                            Text(COMMON_CURRENCIES[code] ?: "", color = text_muted, fontSize = 11.sp)
                        }
                        Text(String.format(trLocale, "%.4f", r[code]!!), color = result_color, fontSize = 14.sp, fontWeight = FontWeight.Bold)
                    }
                    Divider(color = border, thickness = 0.5.dp)
                }
            } ?: run {
                if (!isLoading) {
                    Text("Kurlar yüklenemedi. İnternet bağlantınızı kontrol edin veya 'Dönüştür' butonuna basın.", color = text_muted, fontSize = 13.sp)
                }
            }
        }
    }

    if (showFromPicker) {
        CurrencyPickerDialog(
            currencies = COMMON_CURRENCIES,
            selected = fromCurr,
            onSelect = {
                fromCurr = it
                showFromPicker = false
                result = null
            },
            onDismiss = { showFromPicker = false }
        )
    }

    if (showToPicker) {
        CurrencyPickerDialog(
            currencies = COMMON_CURRENCIES,
            selected = toCurr,
            onSelect = {
                toCurr = it
                showToPicker = false
                result = null
            },
            onDismiss = { showToPicker = false }
        )
    }
}

@Composable
fun CurrencyPickerDialog(
    currencies: Map<String, String>,
    selected: String,
    onSelect: (String) -> Unit,
    onDismiss: () -> Unit
) {
    Dialog(onDismissRequest = onDismiss) {
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(16.dp))
                .background(bg_secondary)
                .border(1.dp, border, RoundedCornerShape(16.dp))
                .padding(20.dp)
        ) {
            Column {
                Row(
                    modifier = Modifier.fillMaxWidth(),
                    horizontalArrangement = Arrangement.SpaceBetween,
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Text("Para Birimi Seç", color = text_main, fontSize = 18.sp, fontWeight = FontWeight.Bold)
                    IconButton(onClick = onDismiss) {
                        Icon(Icons.Default.Close, contentDescription = null, tint = text_muted)
                    }
                }
                Divider(color = border, thickness = 0.5.dp, modifier = Modifier.padding(vertical = 8.dp))
                LazyColumn(modifier = Modifier.heightIn(max = 350.dp)) {
                    items(currencies.keys.toList()) { code ->
                        val isSel = code == selected
                        Row(
                            modifier = Modifier
                                .fillMaxWidth()
                                .clip(RoundedCornerShape(8.dp))
                                .background(if (isSel) bg_input else Color.Transparent)
                                .clickable { onSelect(code) }
                                .padding(horizontal = 12.dp, vertical = 10.dp),
                            horizontalArrangement = Arrangement.SpaceBetween,
                            verticalAlignment = Alignment.CenterVertically
                        ) {
                            Column {
                                Text(code, color = if (isSel) accent else text_main, fontWeight = FontWeight.Bold, fontSize = 15.sp)
                                Text(currencies[code] ?: "", color = text_muted, fontSize = 12.sp)
                            }
                            if (isSel) {
                                Icon(Icons.Default.Check, contentDescription = null, tint = accent, modifier = Modifier.size(18.dp))
                            }
                        }
                    }
                }
            }
        }
    }
}

// --- PERCENT SCREEN ---
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun PercentScreen() {
    var valA by remember { mutableStateOf("1000") }
    var valB by remember { mutableStateOf("20") }
    var modeIdx by remember { mutableStateOf(0) }
    var res by remember { mutableStateOf<Double?>(null) }

    fun calc() {
        res = calculatePercentage(modeIdx, valA.toDoubleOrNull() ?: 0.0, valB.toDoubleOrNull() ?: 0.0)
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(bg_main)
            .verticalScroll(rememberScrollState())
            .padding(20.dp)
    ) {
        Text("Yüzde Hesaplayıcı", fontSize = 24.sp, fontWeight = FontWeight.Bold, color = text_main)
        Text("Hızlı Hesaplamalar 📊", fontSize = 14.sp, color = text_muted, modifier = Modifier.padding(top = 4.dp, bottom = 20.dp))

        Column(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(15.dp))
                .background(bg_secondary)
                .border(1.dp, border, RoundedCornerShape(15.dp))
                .padding(15.dp)
                .padding(bottom = 20.dp)
        ) {
            PERCENT_MODES.forEachIndexed { i, mode ->
                Box(
                    modifier = Modifier
                        .fillMaxWidth()
                        .clickable { modeIdx = i }
                        .background(if (modeIdx == i) bg_input else Color.Transparent)
                        .padding(12.dp)
                ) {
                    Text(mode, color = if (modeIdx == i) accent else text_muted, fontWeight = if (modeIdx == i) FontWeight.Bold else FontWeight.Normal)
                }
                Divider(color = border, thickness = 0.5.dp)
            }
        }

        Spacer(modifier = Modifier.height(15.dp))

        Column(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(15.dp))
                .background(bg_secondary)
                .border(1.dp, border, RoundedCornerShape(15.dp))
                .padding(15.dp)
        ) {
            Row(modifier = Modifier.padding(bottom = 15.dp), horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                Column(modifier = Modifier.weight(1f)) {
                    Text("Değer A", fontSize = 11.sp, fontWeight = FontWeight.Bold, color = text_muted, modifier = Modifier.padding(bottom = 5.dp))
                    OutlinedTextField(
                        value = valA, onValueChange = { valA = it },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = TextFieldDefaults.outlinedTextFieldColors(
                            containerColor = bg_input, focusedTextColor = text_main, unfocusedTextColor = text_main,
                            focusedBorderColor = border, unfocusedBorderColor = border
                        )
                    )
                }
                Column(modifier = Modifier.weight(1f)) {
                    Text("Değer B", fontSize = 11.sp, fontWeight = FontWeight.Bold, color = text_muted, modifier = Modifier.padding(bottom = 5.dp))
                    OutlinedTextField(
                        value = valB, onValueChange = { valB = it },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = TextFieldDefaults.outlinedTextFieldColors(
                            containerColor = bg_input, focusedTextColor = text_main, unfocusedTextColor = text_main,
                            focusedBorderColor = border, unfocusedBorderColor = border
                        )
                    )
                }
            }
            Button(
                onClick = { calc() },
                modifier = Modifier.fillMaxWidth().height(50.dp),
                colors = ButtonDefaults.buttonColors(containerColor = accent)
            ) {
                Text("Hesapla", color = text_main, fontWeight = FontWeight.Bold, fontSize = 16.sp)
            }
        }

        if (res != null) {
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(top = 20.dp)
                    .clip(RoundedCornerShape(12.dp))
                    .background(bg_input)
                    .border(width = 4.dp, color = accent)
                    .padding(15.dp),
                contentAlignment = Alignment.Center
            ) {
                Column(horizontalAlignment = Alignment.CenterHorizontally) {
                    Text("Sonuç", color = text_muted, fontSize = 12.sp, modifier = Modifier.padding(bottom = 5.dp))
                    Text(String.format(trLocale, "%,.2f", res), color = result_color, fontSize = 20.sp, fontWeight = FontWeight.Bold)
                }
            }
        }
    }
}

// --- TAX SCREEN ---
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun TaxScreen() {
    var brutAylik by remember { mutableStateOf("50000") }
    var primAylik by remember { mutableStateOf("2000") }
    var result by remember { mutableStateOf<TaxResult?>(null) }

    fun calc() {
        result = runCalculation(brutAylik.toDoubleOrNull() ?: 0.0, primAylik.toDoubleOrNull() ?: 0.0, false)
    }

    LaunchedEffect(Unit) { calc() }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(bg_main)
            .verticalScroll(rememberScrollState())
            .padding(20.dp)
    ) {
        Text("${CURRENT_YEAR} YILI ÖZETİ", fontSize = 24.sp, fontWeight = FontWeight.Bold, color = text_main)
        Text("Vergi İadeleri ✨", fontSize = 14.sp, color = text_muted, modifier = Modifier.padding(top = 4.dp, bottom = 20.dp))

        Column(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(15.dp))
                .background(bg_secondary)
                .border(1.dp, border, RoundedCornerShape(15.dp))
                .padding(15.dp)
        ) {
            Row(modifier = Modifier.padding(bottom = 15.dp), horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                Column(modifier = Modifier.weight(1f)) {
                    Text("Brüt Aylık (₺)", fontSize = 11.sp, fontWeight = FontWeight.Bold, color = text_muted, modifier = Modifier.padding(bottom = 5.dp))
                    OutlinedTextField(
                        value = brutAylik, onValueChange = { brutAylik = it },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = TextFieldDefaults.outlinedTextFieldColors(
                            containerColor = bg_input, focusedTextColor = text_main, unfocusedTextColor = text_main,
                            focusedBorderColor = border, unfocusedBorderColor = border
                        )
                    )
                }
                Column(modifier = Modifier.weight(1f)) {
                    Text("Prim (₺)", fontSize = 11.sp, fontWeight = FontWeight.Bold, color = text_muted, modifier = Modifier.padding(bottom = 5.dp))
                    OutlinedTextField(
                        value = primAylik, onValueChange = { primAylik = it },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = TextFieldDefaults.outlinedTextFieldColors(
                            containerColor = bg_input, focusedTextColor = text_main, unfocusedTextColor = text_main,
                            focusedBorderColor = border, unfocusedBorderColor = border
                        )
                    )
                }
            }
            Button(
                onClick = { calc() },
                modifier = Modifier.fillMaxWidth().height(50.dp),
                colors = ButtonDefaults.buttonColors(containerColor = accent)
            ) {
                Text("Hesapla", color = text_main, fontWeight = FontWeight.Bold, fontSize = 16.sp)
            }
        }

        result?.let { r ->
            Row(modifier = Modifier.fillMaxWidth().padding(vertical = 20.dp), horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                SummaryCard(
                    title = "Toplam Prim",
                    value = "₺ ${trNumberFormat.format(r.toplamPrim.toLong())}",
                    icon = { Icon(Icons.Default.AccountBalanceWallet, contentDescription = null, tint = accent, modifier = Modifier.size(15.dp)) },
                    modifier = Modifier.weight(1f)
                )
                SummaryCard(
                    title = "Vergi İadesi",
                    value = "₺ ${trNumberFormat.format(r.vergiIadesi.toLong())}",
                    icon = { Icon(Icons.Default.Calculate, contentDescription = null, tint = text_muted, modifier = Modifier.size(15.dp)) },
                    modifier = Modifier.weight(1f)
                )
                SummaryCard(
                    title = "Vergi Dilimi",
                    value = r.vergiDilimi,
                    icon = { Icon(Icons.Default.TrendingUp, contentDescription = null, tint = result_color, modifier = Modifier.size(15.dp)) },
                    modifier = Modifier.weight(1f)
                )
            }

            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .clip(RoundedCornerShape(15.dp))
                    .background(bg_secondary)
                    .border(1.dp, border, RoundedCornerShape(15.dp))
                    .padding(15.dp)
            ) {
                Text("Aylık İade Dağılımı", color = text_main, fontSize = 18.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 15.dp))
                Canvas(modifier = Modifier.fillMaxWidth().height(200.dp)) {
                    val maxVal = r.aylikIadeListesi.maxOfOrNull { it.first } ?: 1.0
                    val barWidth = size.width / (r.aylikIadeListesi.size * 2)
                    r.aylikIadeListesi.forEachIndexed { i, (iade, _) ->
                        val h = (iade / maxVal) * size.height
                        val x = i * (barWidth * 2) + barWidth / 2
                        drawRect(
                            color = accent,
                            topLeft = Offset(x.toFloat(), (size.height - h).toFloat()),
                            size = androidx.compose.ui.geometry.Size(barWidth.toFloat(), h.toFloat())
                        )
                    }
                }
            }

            Column(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(top = 20.dp)
                    .clip(RoundedCornerShape(15.dp))
                    .background(bg_secondary)
                    .border(1.dp, border, RoundedCornerShape(15.dp))
                    .padding(15.dp)
            ) {
                Text("Aylık Detaylar", color = text_main, fontSize = 18.sp, fontWeight = FontWeight.Bold, modifier = Modifier.padding(bottom = 15.dp))
                val currentMonth = Calendar.getInstance().get(Calendar.MONTH)
                r.ayIsimleri.forEachIndexed { i, ay ->
                    Row(modifier = Modifier.fillMaxWidth().padding(vertical = 12.dp), horizontalArrangement = Arrangement.SpaceBetween, verticalAlignment = Alignment.CenterVertically) {
                        Text(ay, color = text_main, fontSize = 14.sp, modifier = Modifier.width(60.dp))
                        Row(modifier = Modifier.weight(1f), verticalAlignment = Alignment.CenterVertically) {
                            Text(String.format(trLocale, "₺ %,.0f", r.aylikIadeListesi[i].first), color = result_color, fontSize = 14.sp, fontWeight = FontWeight.Bold)
                            Text(" (%${(r.aylikIadeListesi[i].second * 100).toInt()})", color = text_muted, fontSize = 11.sp, modifier = Modifier.padding(start = 8.dp))
                        }
                        Box(modifier = Modifier.clip(RoundedCornerShape(12.dp)).background(if (i < currentMonth) Color(0xFF16A34A) else bg_input).padding(horizontal = 10.dp, vertical = 4.dp)) {
                            Text(if (i < currentMonth) "Ödendi" else "Bekliyor", color = text_main, fontSize = 10.sp, fontWeight = FontWeight.Bold)
                        }
                    }
                    Divider(color = border, thickness = 0.5.dp)
                }
            }
        }
    }
}

// --- COMPOUND AREA CHART ---
@Composable
fun CompoundAreaChart(
    rows: List<YearRow>,
    highRows: List<YearRow>?,
    lowRows: List<YearRow>?
) {
    if (rows.isEmpty()) return

    val step = maxOf(1, rows.size / 20)
    val sampled = rows.filterIndexed { i, _ -> i % step == 0 || i == rows.size - 1 }
    val sampledHigh = highRows?.filterIndexed { i, _ -> i % step == 0 || i == highRows.size - 1 }
    val sampledLow = lowRows?.filterIndexed { i, _ -> i % step == 0 || i == lowRows.size - 1 }

    val allValues = sampled.map { it.balance } + sampled.map { it.totalContrib } +
            (sampledHigh?.map { it.balance } ?: emptyList()) +
            (sampledLow?.map { it.balance } ?: emptyList())

    val minVal = maxOf(0.0, (allValues.minOrNull() ?: 0.0) * 0.95)
    val maxVal = (allValues.maxOrNull() ?: 1.0) * 1.05

    Column(
        modifier = Modifier
            .fillMaxWidth()
            .clip(RoundedCornerShape(15.dp))
            .background(bg_secondary)
            .border(1.dp, border, RoundedCornerShape(15.dp))
            .padding(15.dp)
    ) {
        Text(
            "Yıllara Göre Bakiye",
            color = text_main,
            fontSize = 18.sp,
            fontWeight = FontWeight.Bold,
            modifier = Modifier.padding(bottom = 12.dp)
        )

        // Legend Row
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(bottom = 12.dp),
            horizontalArrangement = Arrangement.Center,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Box(modifier = Modifier.size(9.dp).clip(CircleShape).background(accent))
            Spacer(modifier = Modifier.width(5.dp))
            Text("Bakiye", color = text_main, fontSize = 11.sp)

            Spacer(modifier = Modifier.width(14.dp))
            Box(modifier = Modifier.size(9.dp).clip(CircleShape).background(Color(0xFF22C55E)))
            Spacer(modifier = Modifier.width(5.dp))
            Text("Katkı", color = text_main, fontSize = 11.sp)

            if (sampledHigh != null) {
                Spacer(modifier = Modifier.width(14.dp))
                Box(modifier = Modifier.size(9.dp).clip(CircleShape).background(Color(0xFFA855F7)))
                Spacer(modifier = Modifier.width(5.dp))
                Text("Yüksek", color = text_main, fontSize = 11.sp)
            }

            if (sampledLow != null) {
                Spacer(modifier = Modifier.width(14.dp))
                Box(modifier = Modifier.size(9.dp).clip(CircleShape).background(Color(0xFFEF4444)))
                Spacer(modifier = Modifier.width(5.dp))
                Text("Düşük", color = text_main, fontSize = 11.sp)
            }
        }

        Canvas(
            modifier = Modifier
                .fillMaxWidth()
                .height(220.dp)
        ) {
            val chartLeft = 56.dp.toPx()
            val chartRight = size.width - 6.dp.toPx()
            val chartTop = 10.dp.toPx()
            val chartBottom = size.height - 24.dp.toPx()
            val chartWidth = chartRight - chartLeft
            val chartHeight = chartBottom - chartTop

            val ySteps = 4
            val yTextPaint = android.graphics.Paint().apply {
                color = android.graphics.Color.parseColor("#A9D6E5")
                textSize = 25f
                textAlign = android.graphics.Paint.Align.RIGHT
                isAntiAlias = true
            }
            val xTextPaint = android.graphics.Paint().apply {
                color = android.graphics.Color.parseColor("#A9D6E5")
                textSize = 25f
                textAlign = android.graphics.Paint.Align.CENTER
                isAntiAlias = true
            }

            val dashGrid = PathEffect.dashPathEffect(floatArrayOf(8f, 8f), 0f)

            // Horizontal Grid Lines & Y-Labels
            for (i in 0..ySteps) {
                val fraction = i.toFloat() / ySteps
                val yPos = chartBottom - (fraction * chartHeight)
                val valAtStep = minVal + fraction * (maxVal - minVal)

                drawLine(
                    color = border.copy(alpha = 0.5f),
                    start = Offset(chartLeft, yPos),
                    end = Offset(chartRight, yPos),
                    strokeWidth = 1.dp.toPx(),
                    pathEffect = dashGrid
                )

                val labelStr = String.format(Locale.US, "%.0f", valAtStep)
                drawContext.canvas.nativeCanvas.drawText(
                    labelStr,
                    chartLeft - 6.dp.toPx(),
                    yPos + 9f,
                    yTextPaint
                )
            }

            // Vertical Grid Lines & X-Labels
            val count = sampled.size
            for (i in 0 until count) {
                val xPos = chartLeft + (i.toFloat() / maxOf(count - 1, 1)) * chartWidth

                drawLine(
                    color = border.copy(alpha = 0.3f),
                    start = Offset(xPos, chartTop),
                    end = Offset(xPos, chartBottom),
                    strokeWidth = 1.dp.toPx(),
                    pathEffect = dashGrid
                )

                val yearStr = "${sampled[i].year}"
                drawContext.canvas.nativeCanvas.drawText(
                    yearStr,
                    xPos,
                    size.height - 4.dp.toPx(),
                    xTextPaint
                )
            }

            fun calculateOffsets(values: List<Double>): List<Offset> {
                val span = maxOf(maxVal - minVal, 1.0)
                return values.mapIndexed { idx, v ->
                    val x = chartLeft + (idx.toFloat() / maxOf(values.size - 1, 1)) * chartWidth
                    val y = chartBottom - (((v - minVal) / span).toFloat() * chartHeight)
                    Offset(x, y)
                }
            }

            fun createSmoothPath(points: List<Offset>): Path {
                val path = Path()
                if (points.isEmpty()) return path
                path.moveTo(points[0].x, points[0].y)
                for (i in 0 until points.size - 1) {
                    val p0 = if (i > 0) points[i - 1] else points[i]
                    val p1 = points[i]
                    val p2 = points[i + 1]
                    val p3 = if (i + 2 < points.size) points[i + 2] else p2

                    val cp1x = p1.x + (p2.x - p0.x) * 0.18f
                    val cp1y = p1.y + (p2.y - p0.y) * 0.18f
                    val cp2x = p2.x - (p3.x - p1.x) * 0.18f
                    val cp2y = p2.y - (p3.y - p1.y) * 0.18f

                    path.cubicTo(cp1x, cp1y, cp2x, cp2y, p2.x, p2.y)
                }
                return path
            }

            fun createAreaPath(points: List<Offset>, bottomY: Float): Path {
                val path = Path()
                if (points.isEmpty()) return path
                path.moveTo(points.first().x, bottomY)
                path.lineTo(points.first().x, points.first().y)
                for (i in 0 until points.size - 1) {
                    val p0 = if (i > 0) points[i - 1] else points[i]
                    val p1 = points[i]
                    val p2 = points[i + 1]
                    val p3 = if (i + 2 < points.size) points[i + 2] else p2

                    val cp1x = p1.x + (p2.x - p0.x) * 0.18f
                    val cp1y = p1.y + (p2.y - p0.y) * 0.18f
                    val cp2x = p2.x - (p3.x - p1.x) * 0.18f
                    val cp2y = p2.y - (p3.y - p1.y) * 0.18f

                    path.cubicTo(cp1x, cp1y, cp2x, cp2y, p2.x, p2.y)
                }
                path.lineTo(points.last().x, bottomY)
                path.close()
                return path
            }

            val bakiyePoints = calculateOffsets(sampled.map { it.balance })
            val contribPoints = calculateOffsets(sampled.map { it.totalContrib })

            // 1. Draw gradient area under Bakiye
            val areaPath = createAreaPath(bakiyePoints, chartBottom)
            drawPath(
                path = areaPath,
                brush = Brush.verticalGradient(
                    colors = listOf(
                        accent.copy(alpha = 0.35f),
                        accent.copy(alpha = 0.02f)
                    ),
                    startY = chartTop,
                    endY = chartBottom
                )
            )

            // 2. Draw Yüksek curve (purple)
            if (sampledHigh != null) {
                val highPoints = calculateOffsets(sampledHigh.map { it.balance })
                drawPath(
                    path = createSmoothPath(highPoints),
                    color = Color(0xFFA855F7),
                    style = Stroke(width = 2.dp.toPx(), cap = StrokeCap.Round)
                )
            }

            // 3. Draw Düşük curve (red)
            if (sampledLow != null) {
                val lowPoints = calculateOffsets(sampledLow.map { it.balance })
                drawPath(
                    path = createSmoothPath(lowPoints),
                    color = Color(0xFFEF4444),
                    style = Stroke(width = 2.dp.toPx(), cap = StrokeCap.Round)
                )
            }

            // 4. Draw Katkı curve (green)
            drawPath(
                path = createSmoothPath(contribPoints),
                color = Color(0xFF22C55E),
                style = Stroke(width = 2.2.dp.toPx(), cap = StrokeCap.Round)
            )

            // 5. Draw Bakiye curve (cyan)
            drawPath(
                path = createSmoothPath(bakiyePoints),
                color = accent,
                style = Stroke(width = 2.8.dp.toPx(), cap = StrokeCap.Round)
            )
        }
    }
}

// --- COMPOUND SCREEN ---
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun CompoundScreen() {
    val FREQS = FREQ_MAP.keys.toList()
    var principal by remember { mutableStateOf("0") }
    var monthly by remember { mutableStateOf("5000") }
    var years by remember { mutableStateOf("10") }
    var rate by remember { mutableStateOf("10") }
    var variance by remember { mutableStateOf("3") }
    var freqIdx by remember { mutableStateOf(0) }
    var rows by remember { mutableStateOf<List<YearRow>>(emptyList()) }
    var lowRows by remember { mutableStateOf<List<YearRow>?>(null) }
    var highRows by remember { mutableStateOf<List<YearRow>?>(null) }
    var tableVisible by remember { mutableStateOf(false) }

    fun calc() {
        val p = principal.toDoubleOrNull() ?: 0.0
        val mo = monthly.toDoubleOrNull() ?: 0.0
        val yr = years.toIntOrNull() ?: 10
        val rt = rate.toDoubleOrNull() ?: 0.0
        val vr = variance.toDoubleOrNull() ?: 0.0
        val n = FREQ_MAP[FREQS[freqIdx]] ?: 365
        rows = compoundFV(p, mo, yr, rt, n)
        lowRows = if (vr > 0) compoundFV(p, mo, yr, maxOf(0.0, rt - vr), n) else null
        highRows = if (vr > 0) compoundFV(p, mo, yr, rt + vr, n) else null
    }

    fun reset() {
        principal = "0"; monthly = "5000"; years = "10"; rate = "10"; variance = "3"; freqIdx = 0
        rows = emptyList(); lowRows = null; highRows = null
    }

    LaunchedEffect(Unit) {
        if (rows.isEmpty()) calc()
    }

    val fmt = { n: Double -> trNumberFormat.format(n.toLong()) }
    val hasResult = rows.isNotEmpty()
    val finalBal = if (hasResult) rows.last().balance else 0.0
    val totalContrib = if (hasResult) rows.last().totalContrib else 0.0
    val totalProfit = if (hasResult) rows.last().profit else 0.0

    Column(
        modifier = Modifier
            .fillMaxSize()
            .background(bg_main)
            .verticalScroll(rememberScrollState())
            .padding(20.dp)
    ) {
        Text("Bileşik Kar Hesaplayıcısı", fontSize = 24.sp, fontWeight = FontWeight.Bold, color = text_main)
        Text("Uzun vadeli büyüme 📈", fontSize = 14.sp, color = text_muted, modifier = Modifier.padding(top = 4.dp, bottom = 20.dp))

        // Giriş Formu
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .clip(RoundedCornerShape(15.dp))
                .background(bg_secondary)
                .border(1.dp, border, RoundedCornerShape(15.dp))
                .padding(15.dp)
        ) {
            Row(modifier = Modifier.padding(bottom = 15.dp), horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                Column(modifier = Modifier.weight(1f)) {
                    Text("İlk Yatırım", fontSize = 11.sp, fontWeight = FontWeight.Bold, color = text_muted, modifier = Modifier.padding(bottom = 5.dp))
                    OutlinedTextField(
                        value = principal, onValueChange = { principal = it },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = TextFieldDefaults.outlinedTextFieldColors(containerColor = bg_input, focusedTextColor = text_main, unfocusedTextColor = text_main)
                    )
                }
                Column(modifier = Modifier.weight(1f)) {
                    Text("Aylık Katkı", fontSize = 11.sp, fontWeight = FontWeight.Bold, color = text_muted, modifier = Modifier.padding(bottom = 5.dp))
                    OutlinedTextField(
                        value = monthly, onValueChange = { monthly = it },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = TextFieldDefaults.outlinedTextFieldColors(containerColor = bg_input, focusedTextColor = text_main, unfocusedTextColor = text_main)
                    )
                }
            }
            Row(modifier = Modifier.padding(bottom = 15.dp), horizontalArrangement = Arrangement.spacedBy(10.dp)) {
                Column(modifier = Modifier.weight(1f)) {
                    Text("Süre (Yıl)", fontSize = 11.sp, fontWeight = FontWeight.Bold, color = text_muted, modifier = Modifier.padding(bottom = 5.dp))
                    OutlinedTextField(
                        value = years, onValueChange = { years = it },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = TextFieldDefaults.outlinedTextFieldColors(containerColor = bg_input, focusedTextColor = text_main, unfocusedTextColor = text_main)
                    )
                }
                Column(modifier = Modifier.weight(1f)) {
                    Text("Kar Oranı (%)", fontSize = 11.sp, fontWeight = FontWeight.Bold, color = text_muted, modifier = Modifier.padding(bottom = 5.dp))
                    OutlinedTextField(
                        value = rate, onValueChange = { rate = it },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = TextFieldDefaults.outlinedTextFieldColors(containerColor = bg_input, focusedTextColor = text_main, unfocusedTextColor = text_main)
                    )
                }
            }
            Row(modifier = Modifier.padding(bottom = 15.dp), horizontalArrangement = Arrangement.spacedBy(10.dp), verticalAlignment = Alignment.CenterVertically) {
                Column(modifier = Modifier.weight(0.4f)) {
                    Text("Varyans (±%)", fontSize = 11.sp, fontWeight = FontWeight.Bold, color = text_muted, modifier = Modifier.padding(bottom = 5.dp))
                    OutlinedTextField(
                        value = variance, onValueChange = { variance = it },
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        colors = TextFieldDefaults.outlinedTextFieldColors(containerColor = bg_input, focusedTextColor = text_main, unfocusedTextColor = text_main)
                    )
                }
                Column(modifier = Modifier.weight(0.6f)) {
                    Text("Bileşik Frekans", fontSize = 11.sp, fontWeight = FontWeight.Bold, color = text_muted, modifier = Modifier.padding(bottom = 5.dp))
                    Row(modifier = Modifier.horizontalScroll(rememberScrollState()), horizontalArrangement = Arrangement.spacedBy(6.dp)) {
                        FREQS.forEachIndexed { i, f ->
                            Box(
                                modifier = Modifier
                                    .clip(RoundedCornerShape(16.dp))
                                    .background(if (freqIdx == i) accent else bg_input)
                                    .border(1.dp, border, RoundedCornerShape(16.dp))
                                    .clickable { freqIdx = i }
                                    .padding(horizontal = 10.dp, vertical = 6.dp)
                            ) {
                                Text(f, color = if (freqIdx == i) text_main else text_muted, fontSize = 12.sp, fontWeight = if (freqIdx == i) FontWeight.Bold else FontWeight.Normal)
                            }
                        }
                    }
                }
            }
            Row(horizontalArrangement = Arrangement.spacedBy(8.dp)) {
                Button(
                    onClick = { calc() },
                    modifier = Modifier.weight(1f).height(50.dp),
                    colors = ButtonDefaults.buttonColors(containerColor = accent)
                ) {
                    Text("Hesapla", color = text_main, fontWeight = FontWeight.Bold, fontSize = 16.sp)
                }
                OutlinedButton(
                    onClick = { reset() },
                    modifier = Modifier.weight(0.5f).height(50.dp),
                    colors = ButtonDefaults.outlinedButtonColors(contentColor = text_muted),
                    border = androidx.compose.foundation.BorderStroke(1.dp, border)
                ) {
                    Text("Sıfırla")
                }
            }
        }

        // Sonuç Kartları & Grafik
        if (hasResult) {
            // Row 1: Nihai Bakiye, Toplam Katkı, Kar
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(top = 20.dp, bottom = 10.dp),
                horizontalArrangement = Arrangement.spacedBy(8.dp)
            ) {
                SummaryCard(
                    title = "Nihai Bakiye",
                    value = "₺ ${fmt(finalBal)}",
                    icon = { Icon(Icons.Default.AccountBalanceWallet, contentDescription = null, tint = accent, modifier = Modifier.size(15.dp)) },
                    modifier = Modifier.weight(1f)
                )
                SummaryCard(
                    title = "Toplam Katkı",
                    value = "₺ ${fmt(totalContrib)}",
                    icon = { Icon(Icons.Default.Calculate, contentDescription = null, tint = text_muted, modifier = Modifier.size(15.dp)) },
                    modifier = Modifier.weight(1f)
                )
                SummaryCard(
                    title = "Kar",
                    value = "₺ ${fmt(totalProfit)}",
                    icon = { Icon(Icons.Default.TrendingUp, contentDescription = null, tint = result_color, modifier = Modifier.size(15.dp)) },
                    modifier = Modifier.weight(1f)
                )
            }

            // Row 2: Düşük, Yüksek
            if (highRows != null && lowRows != null) {
                Row(
                    modifier = Modifier
                        .fillMaxWidth()
                        .padding(bottom = 15.dp),
                    horizontalArrangement = Arrangement.spacedBy(8.dp)
                ) {
                    SummaryCard(
                        title = "Düşük (−$variance%)",
                        value = "₺ ${fmt(lowRows!!.last().balance)}",
                        icon = { Icon(Icons.Default.Info, contentDescription = null, tint = Color(0xFFEF4444), modifier = Modifier.size(15.dp)) },
                        modifier = Modifier.weight(1f)
                    )
                    SummaryCard(
                        title = "Yüksek (+$variance%)",
                        value = "₺ ${fmt(highRows!!.last().balance)}",
                        icon = { Icon(Icons.Default.Info, contentDescription = null, tint = Color(0xFFA855F7), modifier = Modifier.size(15.dp)) },
                        modifier = Modifier.weight(1f)
                    )
                }
            }

            // Area Chart
            CompoundAreaChart(rows = rows, highRows = highRows, lowRows = lowRows)

            // Yıllık Tablo Butonu
            Button(
                onClick = { tableVisible = true },
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(top = 16.dp)
                    .height(50.dp),
                colors = ButtonDefaults.buttonColors(containerColor = bg_secondary),
                border = androidx.compose.foundation.BorderStroke(1.dp, border)
            ) {
                Text("Yıllık Tabloyu Gör", color = accent, fontWeight = FontWeight.Bold)
            }
        }
    }

    if (tableVisible) {
        Dialog(onDismissRequest = { tableVisible = false }) {
            Box(
                modifier = Modifier
                    .fillMaxWidth()
                    .clip(RoundedCornerShape(20.dp))
                    .background(bg_secondary)
                    .border(1.dp, border, RoundedCornerShape(20.dp))
                    .padding(20.dp)
            ) {
                Column {
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween,
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text("Yıllık Bileşik Kar Dökümü", color = text_main, fontSize = 16.sp, fontWeight = FontWeight.Bold)
                        IconButton(onClick = { tableVisible = false }) {
                            Icon(Icons.Default.Close, contentDescription = null, tint = text_muted)
                        }
                    }
                    Divider(color = border, thickness = 1.dp, modifier = Modifier.padding(bottom = 8.dp))
                    Row(modifier = Modifier.fillMaxWidth()) {
                        listOf("Yıl", "Bakiye", "Katkı", "Kar").forEach { h ->
                            Text(h, color = accent, fontWeight = FontWeight.Bold, fontSize = 12.sp, modifier = Modifier.weight(1f), textAlign = androidx.compose.ui.text.style.TextAlign.Center)
                        }
                    }
                    LazyColumn(modifier = Modifier.heightIn(max = 400.dp)) {
                        itemsIndexed(rows) { index, item ->
                            Row(
                                modifier = Modifier
                                    .fillMaxWidth()
                                    .background(if (index % 2 == 0) bg_input else Color.Transparent)
                                    .padding(vertical = 8.dp)
                            ) {
                                Text(item.year.toString(), color = text_main, fontSize = 12.sp, modifier = Modifier.weight(1f), textAlign = androidx.compose.ui.text.style.TextAlign.Center)
                                Text(fmt(item.balance), color = text_main, fontSize = 12.sp, modifier = Modifier.weight(1f), textAlign = androidx.compose.ui.text.style.TextAlign.Center)
                                Text(fmt(item.totalContrib), color = text_main, fontSize = 12.sp, modifier = Modifier.weight(1f), textAlign = androidx.compose.ui.text.style.TextAlign.Center)
                                Text(fmt(item.profit), color = result_color, fontSize = 12.sp, modifier = Modifier.weight(1f), textAlign = androidx.compose.ui.text.style.TextAlign.Center)
                            }
                        }
                    }
                    Button(
                        onClick = { tableVisible = false },
                        modifier = Modifier.fillMaxWidth().padding(top = 12.dp),
                        colors = ButtonDefaults.buttonColors(containerColor = accent)
                    ) {
                        Text("Kapat", color = text_main, fontWeight = FontWeight.Bold)
                    }
                }
            }
        }
    }
}
