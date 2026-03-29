import React, { useState, useEffect } from "react";
import {
  StyleSheet,
  Text,
  View,
  ScrollView,
  TouchableOpacity,
  TextInput,
  Dimensions,
  SafeAreaView,
  StatusBar as RNStatusBar,
  Modal,
  FlatList,
} from "react-native";
import { StatusBar } from "expo-status-bar";
import { NavigationContainer } from "@react-navigation/native";
import { createBottomTabNavigator } from "@react-navigation/bottom-tabs";
import { BarChart, LineChart } from "react-native-chart-kit";
import {
  Wallet,
  TrendingUp,
  FileText,
  ChevronRight,
  Info,
  RefreshCcw,
  Percent,
  Calculator,
  BarChart2,
} from "lucide-react-native";
import {
  THEME,
  runCalculation,
  TaxResult,
  fetchExchangeRates,
  COMMON_CURRENCIES,
  PERCENT_MODES,
  calculatePercentage,
  compoundFV,
  FREQ_MAP,
  YearRow,
} from "./logic";

const screenWidth = Dimensions.get("window").width;
const Tab = createBottomTabNavigator();

// --- COMPONENTS ---

const SummaryCard = ({ title, value, icon }: { title: string, value: string, icon: any }) => (
  <View style={styles.card}>
    <View style={styles.cardHeader}>
      {icon}
      <Text style={styles.cardTitle}>{title.toUpperCase()}</Text>
    </View>
    <Text style={styles.cardValue}>{value}</Text>
  </View>
);

// --- SCREENS ---

function CurrencyScreen() {
  const [brutAylik, setBrutAylik] = useState("100");
  const [fromCurr, setFromCurr] = useState("USD");
  const [toCurr, setToCurr] = useState("TRY");
  const [result, setResult] = useState<string | null>(null);
  const [lastAmount, setLastAmount] = useState<string | null>(null);
  const [rates, setRates] = useState<any>(null);

  useEffect(() => {
    loadRates();
  }, []);

  const loadRates = async () => {
    const res = await fetchExchangeRates(fromCurr);
    setRates(res);
    if (res && res[toCurr]) {
      const amount = parseFloat(brutAylik) || 0;
      setResult((amount * res[toCurr]).toFixed(2));
      setLastAmount(amount.toString());
    }
  };

  const handleConvert = () => {
    if (rates && rates[toCurr]) {
      const amount = parseFloat(brutAylik) || 0;
      setResult((amount * rates[toCurr]).toFixed(2));
      setLastAmount(amount.toString());
    } else {
      loadRates();
    }
  };

  const swap = () => {
    setFromCurr(toCurr);
    setToCurr(fromCurr);
    setRates(null);
    setResult(null);
    setLastAmount(null);
  }

  return (
    <SafeAreaView style={styles.container}>
      <ScrollView contentContainerStyle={styles.scrollContent}>
        <View style={styles.header}>
          <Text style={styles.headerTitle}>Döviz Dönüştürücü</Text>
          <Text style={styles.headerSubtitle}>Canlı Kurlar 💱</Text>
        </View>

        <View style={styles.inputSection}>
          <View style={styles.inputCard}>
            <Text style={styles.inputLabel}>Miktar</Text>
            <TextInput
              style={styles.textInput}
              value={brutAylik}
              onChangeText={(text) => {
                setBrutAylik(text);
                if (result) setResult(null);
              }}
              keyboardType="numeric"
              placeholderTextColor={THEME.text_muted}
            />
          </View>

          <View style={styles.inputRow}>
            <TouchableOpacity style={styles.currencySelect}>
              <Text style={styles.currencySelectText}>{fromCurr}</Text>
            </TouchableOpacity>
            <TouchableOpacity onPress={swap} style={styles.swapButton}>
              <RefreshCcw color={THEME.accent} size={20} />
            </TouchableOpacity>
            <TouchableOpacity style={styles.currencySelect}>
              <Text style={styles.currencySelectText}>{toCurr}</Text>
            </TouchableOpacity>
          </View>

          <TouchableOpacity style={styles.calcButton} onPress={handleConvert}>
            <Text style={styles.calcButtonText}>Dönüştür</Text>
          </TouchableOpacity>
        </View>

        {result && lastAmount && (
          <View style={styles.resultBox}>
            <Text style={styles.resultValue}>{lastAmount} {fromCurr} = {result} {toCurr}</Text>
          </View>
        )}

        <View style={styles.listSection}>
          <Text style={styles.sectionTitle}>Popüler Kurlar (1 {fromCurr})</Text>
          {rates && Object.keys(COMMON_CURRENCIES).map(code => (
            code !== fromCurr && rates[code] && (
              <View key={code} style={styles.listRow}>
                <Text style={styles.listAy}>{code}</Text>
                <Text style={styles.listIade}>{rates[code].toFixed(4)}</Text>
              </View>
            )
          ))}
        </View>
      </ScrollView>
    </SafeAreaView>
  );
}

function PercentScreen() {
  const [valA, setValA] = useState("1000");
  const [valB, setValB] = useState("20");
  const [modeIdx, setModeIdx] = useState(0);
  const [res, setRes] = useState<number | null>(null);

  const calculate = () => {
    const result = calculatePercentage(modeIdx, parseFloat(valA) || 0, parseFloat(valB) || 0);
    setRes(result);
  };

  return (
    <SafeAreaView style={styles.container}>
      <ScrollView contentContainerStyle={styles.scrollContent}>
        <View style={styles.header}>
          <Text style={styles.headerTitle}>Yüzde Hesaplayıcı</Text>
          <Text style={styles.headerSubtitle}>Hızlı Hesaplamalar 📊</Text>
        </View>

        <View style={styles.listSection}>
          {PERCENT_MODES.map((mode, i) => (
            <TouchableOpacity key={i} onPress={() => setModeIdx(i)} style={[styles.modeButton, modeIdx === i && styles.modeButtonActive]}>
              <Text style={[styles.modeButtonText, modeIdx === i && styles.modeButtonTextActive]}>{mode}</Text>
            </TouchableOpacity>
          ))}
        </View>

        <View style={styles.inputSection}>
          <View style={styles.inputRow}>
            <View style={styles.inputCard}>
              <Text style={styles.inputLabel}>Değer A</Text>
              <TextInput style={styles.textInput} value={valA} onChangeText={setValA} keyboardType="numeric" />
            </View>
            <View style={styles.inputCard}>
              <Text style={styles.inputLabel}>Değer B</Text>
              <TextInput style={styles.textInput} value={valB} onChangeText={setValB} keyboardType="numeric" />
            </View>
          </View>
          <TouchableOpacity style={styles.calcButton} onPress={calculate}>
            <Text style={styles.calcButtonText}>Hesapla</Text>
          </TouchableOpacity>
        </View>

        {res !== null && (
          <View style={styles.resultBox}>
            <Text style={styles.resultValueTitle}>Sonuç</Text>
            <Text style={styles.resultValue}>{res.toLocaleString("tr-TR")}</Text>
          </View>
        )}
      </ScrollView>
    </SafeAreaView>
  );
}

function TaxScreen() {
  const [brutAylik, setBrutAylik] = useState("50000");
  const [primAylik, setPrimAylik] = useState("2000");
  const [isHayat, setIsHayat] = useState(false);
  const [result, setResult] = useState<TaxResult | null>(null);

  useEffect(() => { handleCalculate(); }, []);

  const handleCalculate = () => {
    const res = runCalculation(parseFloat(brutAylik) || 0, parseFloat(primAylik) || 0, isHayat);
    setResult(res);
  };

  if (!result) return null;

  return (
    <SafeAreaView style={styles.container}>
      <ScrollView contentContainerStyle={styles.scrollContent}>
        <View style={styles.header}>
          <Text style={styles.headerTitle}>{result.yil} YILI ÖZETİ</Text>
          <Text style={styles.headerSubtitle}>Vergi İadeleri ✨</Text>
        </View>

        <View style={styles.inputSection}>
          <View style={styles.inputRow}>
            <View style={styles.inputCard}>
              <Text style={styles.inputLabel}>Brüt Aylık (₺)</Text>
              <TextInput style={styles.textInput} value={brutAylik} onChangeText={setBrutAylik} keyboardType="numeric" />
            </View>
            <View style={styles.inputCard}>
              <Text style={styles.inputLabel}>Prim (₺)</Text>
              <TextInput style={styles.textInput} value={primAylik} onChangeText={setPrimAylik} keyboardType="numeric" />
            </View>
          </View>
          <TouchableOpacity style={styles.calcButton} onPress={handleCalculate}>
            <Text style={styles.calcButtonText}>Hesapla</Text>
            <ChevronRight color="#fff" size={20} />
          </TouchableOpacity>
        </View>

        <View style={styles.cardsRow}>
          <SummaryCard title="Toplam Prim" value={`₺ ${result.toplam_prim.toLocaleString("tr-TR")}`} icon={<Calculator color={THEME.accent} size={16} />} />
          <SummaryCard title="Vergi İadesi" value={`₺ ${result.vergi_iadesi.toLocaleString("tr-TR")}`} icon={<TrendingUp color={THEME.result} size={16} />} />
          <SummaryCard title="Vergi Dilimi" value={result.vergi_dilimi} icon={<Info color={THEME.text_muted} size={16} />} />
        </View>

        <View style={styles.chartContainer}>
          <Text style={styles.sectionTitle}>Aylık İade Dağılımı</Text>
          <BarChart
            data={{
              labels: result.ay_isimleri,
              datasets: [{ data: result.aylik_iade_listesi.map(d => d.iade) }]
            }}
            width={screenWidth - 40}
            height={200}
            yAxisLabel="₺"
            yAxisSuffix=""
            chartConfig={chartConfig}
            verticalLabelRotation={30}
            style={styles.chart}
          />
        </View>

        <View style={styles.listSection}>
          <Text style={styles.sectionTitle}>Aylık Detaylar</Text>
          {result.ay_isimleri.map((ay, i) => (
            <View key={i} style={styles.listRow}>
              <Text style={styles.listAy}>{ay}</Text>
              <View style={styles.listValues}>
                <Text style={styles.listIade}>₺ {result.aylik_iade_listesi[i].iade.toFixed(0)}</Text>
                <Text style={styles.listOran}>(%{result.aylik_iade_listesi[i].oran * 100})</Text>
              </View>
              <View style={[styles.statusBadge, { backgroundColor: i < new Date().getMonth() ? "#16a34a" : "#1b263b" }]}>
                <Text style={styles.statusText}>{i < new Date().getMonth() ? "Ödendi" : "Bekliyor"}</Text>
              </View>
            </View>
          ))}
        </View>
      </ScrollView>
    </SafeAreaView>
  );
}

// --- APP ENTRY ---

function CompoundScreen() {
  const FREQS = Object.keys(FREQ_MAP);
  const [principal, setPrincipal] = useState("0");
  const [monthly, setMonthly]     = useState("5000");
  const [years, setYears]         = useState("10");
  const [rate, setRate]           = useState("10");
  const [variance, setVariance]   = useState("3");
  const [freqIdx, setFreqIdx]     = useState(0); // Günlük
  const [rows, setRows]           = useState<YearRow[]>([]);
  const [lowRows, setLowRows]     = useState<YearRow[] | null>(null);
  const [highRows, setHighRows]   = useState<YearRow[] | null>(null);
  const [tableVisible, setTableVisible] = useState(false);

  const calculate = () => {
    const p  = parseFloat(principal)  || 0;
    const mo = parseFloat(monthly)    || 0;
    const yr = parseInt(years)        || 10;
    const rt = parseFloat(rate)       || 0;
    const vr = parseFloat(variance)   || 0;
    const n  = FREQ_MAP[FREQS[freqIdx]];
    setRows(compoundFV(p, mo, yr, rt, n));
    setLowRows(vr > 0  ? compoundFV(p, mo, yr, Math.max(0, rt - vr), n) : null);
    setHighRows(vr > 0 ? compoundFV(p, mo, yr, rt + vr, n)               : null);
  };

  const reset = () => {
    setPrincipal("0"); setMonthly("5000"); setYears("10");
    setRate("10"); setVariance("3"); setFreqIdx(0);
    setRows([]); setLowRows(null); setHighRows(null);
  };

  const fmt = (n: number) => n.toLocaleString("tr-TR", { maximumFractionDigits: 0 });

  const hasResult = rows.length > 0;
  const finalBal     = hasResult ? rows[rows.length - 1].balance     : 0;
  const totalContrib = hasResult ? rows[rows.length - 1].totalContrib : 0;
  const totalProfit  = hasResult ? rows[rows.length - 1].profit       : 0;

  // Chart: en fazla 20 nokta göster
  const step    = hasResult ? Math.max(1, Math.floor(rows.length / 20)) : 1;
  const sampled = hasResult ? rows.filter((_, i) => i % step === 0 || i === rows.length - 1) : [];

  return (
    <SafeAreaView style={styles.container}>
      <ScrollView contentContainerStyle={styles.scrollContent}>
        <View style={styles.header}>
          <Text style={styles.headerTitle}>Bileşik Kar Hesaplayıcısı</Text>
          <Text style={styles.headerSubtitle}>Uzun vadeli büyüme 📈</Text>
        </View>

        {/* Giriş Formu */}
        <View style={styles.inputSection}>
          <View style={styles.inputRow}>
            <View style={styles.inputCard}>
              <Text style={styles.inputLabel}>İlk Yatırım</Text>
              <TextInput style={styles.textInput} value={principal} onChangeText={setPrincipal} keyboardType="numeric" />
            </View>
            <View style={styles.inputCard}>
              <Text style={styles.inputLabel}>Aylık Katkı</Text>
              <TextInput style={styles.textInput} value={monthly} onChangeText={setMonthly} keyboardType="numeric" />
            </View>
          </View>
          <View style={styles.inputRow}>
            <View style={styles.inputCard}>
              <Text style={styles.inputLabel}>Süre (Yıl)</Text>
              <TextInput style={styles.textInput} value={years} onChangeText={setYears} keyboardType="numeric" />
            </View>
            <View style={styles.inputCard}>
              <Text style={styles.inputLabel}>Kar Oranı (%)</Text>
              <TextInput style={styles.textInput} value={rate} onChangeText={setRate} keyboardType="numeric" />
            </View>
          </View>
          <View style={styles.inputRow}>
            <View style={styles.inputCard}>
              <Text style={styles.inputLabel}>Varyans (±%)</Text>
              <TextInput style={styles.textInput} value={variance} onChangeText={setVariance} keyboardType="numeric" />
            </View>
            <View style={[styles.inputCard, { flex: 1 }]}>
              <Text style={styles.inputLabel}>Bileşik Frekans</Text>
              <ScrollView horizontal showsHorizontalScrollIndicator={false}>
                <View style={{ flexDirection: "row", gap: 6 }}>
                  {FREQS.map((f, i) => (
                    <TouchableOpacity
                      key={f}
                      onPress={() => setFreqIdx(i)}
                      style={[styles.chipBtn, freqIdx === i && styles.chipBtnActive]}
                    >
                      <Text style={[styles.chipText, freqIdx === i && styles.chipTextActive]}>{f}</Text>
                    </TouchableOpacity>
                  ))}
                </View>
              </ScrollView>
            </View>
          </View>
          <View style={styles.inputRow}>
            <TouchableOpacity style={[styles.calcButton, { flex: 1, marginRight: 8 }]} onPress={calculate}>
              <Text style={styles.calcButtonText}>Hesapla</Text>
            </TouchableOpacity>
            <TouchableOpacity style={[styles.calcButton, { flex: 0.5, backgroundColor: THEME.bg_input, borderWidth: 1, borderColor: THEME.border }]} onPress={reset}>
              <Text style={[styles.calcButtonText, { color: THEME.text_muted }]}>Sıfırla</Text>
            </TouchableOpacity>
          </View>
        </View>

        {/* Sonuç Kartları */}
        {hasResult && (
          <>
            <View style={styles.cardsRow}>
              <SummaryCard title="Nihai Bakiye"   value={`₺ ${fmt(finalBal)}`}     icon={<Wallet     color={THEME.accent}      size={16} />} />
              <SummaryCard title="Toplam Katkı"  value={`₺ ${fmt(totalContrib)}`}  icon={<Calculator color={THEME.text_muted}  size={16} />} />
              <SummaryCard title="Kar"            value={`₺ ${fmt(totalProfit)}`}   icon={<TrendingUp color={THEME.result}      size={16} />} />
            </View>

            {highRows && lowRows && (
              <View style={styles.cardsRow}>
                <SummaryCard title={`Düşük (−${variance}%)`}   value={`₺ ${fmt(lowRows[lowRows.length-1].balance)}`}   icon={<Info color="#ef4444" size={16} />} />
                <SummaryCard title={`Yüksek (+${variance}%)`} value={`₺ ${fmt(highRows[highRows.length-1].balance)}`} icon={<Info color="#a855f7" size={16} />} />
              </View>
            )}

            {/* Grafiğ */}
            <View style={styles.chartContainer}>
              <Text style={styles.sectionTitle}>Yıllara Göre Bakiye</Text>
              <LineChart
                data={{
                  labels: sampled.map(r => `${r.year}`),
                  datasets: [
                    { data: sampled.map(r => r.balance),      color: () => THEME.accent,  strokeWidth: 2 },
                    { data: sampled.map(r => r.totalContrib), color: () => "#22c55e",     strokeWidth: 2 },
                    ...(highRows && lowRows ? [
                      { data: highRows.filter((_, i) => i % step === 0 || i === highRows.length - 1).map(r => r.balance), color: () => "#a855f7", strokeWidth: 1 },
                      { data: lowRows.filter((_,  i) => i % step === 0 || i === lowRows.length  - 1).map(r => r.balance), color: () => "#ef4444", strokeWidth: 1 },
                    ] : []),
                  ],
                  legend: ["Bakiye", "Katkı", ...(highRows ? ["Yüksek", "Düşük"] : [])],
                }}
                width={screenWidth - 40}
                height={220}
                chartConfig={chartConfig}
                bezier
                style={styles.chart}
                withDots={false}
              />
            </View>

            {/* Yıllık Tablo */}
            <TouchableOpacity
              style={[styles.calcButton, { backgroundColor: THEME.bg_secondary, borderWidth: 1, borderColor: THEME.border, marginBottom: 16 }]}
              onPress={() => setTableVisible(true)}
            >
              <BarChart2 color={THEME.accent} size={18} style={{ marginRight: 8 }} />
              <Text style={[styles.calcButtonText, { color: THEME.accent }]}>Yıllık Tabloyu Gör</Text>
            </TouchableOpacity>

            <Modal visible={tableVisible} animationType="slide" transparent>
              <View style={compoundStyles.modalOverlay}>
                <View style={compoundStyles.modalBox}>
                  <View style={compoundStyles.modalHeader}>
                    <Text style={compoundStyles.modalTitle}>Yıllık Bileşik Kar Dökümü</Text>
                    <TouchableOpacity onPress={() => setTableVisible(false)}>
                      <Text style={compoundStyles.modalClose}>✕</Text>
                    </TouchableOpacity>
                  </View>
                  {/* Tablo Başlık */}
                  <View style={compoundStyles.tableHeader}>
                    {["Yıl", "Bakiye", "Katkı", "Kar"].map(h => (
                      <Text key={h} style={compoundStyles.tableHeaderCell}>{h}</Text>
                    ))}
                  </View>
                  <FlatList
                    data={rows}
                    keyExtractor={item => item.year.toString()}
                    renderItem={({ item, index }) => (
                      <View style={[compoundStyles.tableRow, index % 2 === 0 && compoundStyles.tableRowAlt]}>
                        <Text style={compoundStyles.tableCell}>{item.year}</Text>
                        <Text style={compoundStyles.tableCell}>{fmt(item.balance)}</Text>
                        <Text style={compoundStyles.tableCell}>{fmt(item.totalContrib)}</Text>
                        <Text style={[compoundStyles.tableCell, { color: THEME.result }]}>{fmt(item.profit)}</Text>
                      </View>
                    )}
                  />
                  <TouchableOpacity style={[styles.calcButton, { marginTop: 12 }]} onPress={() => setTableVisible(false)}>
                    <Text style={styles.calcButtonText}>Kapat</Text>
                  </TouchableOpacity>
                </View>
              </View>
            </Modal>
          </>
        )}
      </ScrollView>
    </SafeAreaView>
  );
}

// --- APP ENTRY ---

export default function App() {
  return (
    <NavigationContainer>
      <StatusBar style="light" />
      <Tab.Navigator
        screenOptions={({ route }) => ({
          headerShown: false,
          tabBarStyle: {
            backgroundColor: THEME.bg_secondary,
            borderTopColor: THEME.border,
            borderTopWidth: 1,
            height: 60,
            paddingBottom: 10,
          },
          tabBarActiveTintColor: THEME.accent,
          tabBarInactiveTintColor: THEME.text_muted,
          tabBarIcon: ({ color, size }) => {
            if (route.name === 'Kur')   return <RefreshCcw color={color} size={size} />;
            if (route.name === 'Yüzde') return <Percent    color={color} size={size} />;
            if (route.name === 'Vergi') return <Calculator color={color} size={size} />;
            if (route.name === 'Kar')   return <TrendingUp color={color} size={size} />;
          },
        })}
      >
        <Tab.Screen name="Kur"   component={CurrencyScreen} />
        <Tab.Screen name="Yüzde" component={PercentScreen} />
        <Tab.Screen name="Vergi" component={TaxScreen} />
        <Tab.Screen name="Kar"   component={CompoundScreen} />
      </Tab.Navigator>
    </NavigationContainer>
  );
}

// --- STYLES ---

const chartConfig = {
  backgroundColor: THEME.bg_secondary,
  backgroundGradientFrom: THEME.bg_secondary,
  backgroundGradientTo: THEME.bg_secondary,
  color: (opacity = 1) => `rgba(0, 212, 255, ${opacity})`,
  labelColor: (opacity = 1) => `rgba(169, 214, 229, ${opacity})`,
  decimalPlaces: 0,
};

const styles = StyleSheet.create({
  container: { flex: 1, backgroundColor: THEME.bg_main, paddingTop: RNStatusBar.currentHeight },
  scrollContent: { padding: 20 },
  header: { marginBottom: 20 },
  headerTitle: { fontSize: 24, fontWeight: "bold", color: "#fff" },
  headerSubtitle: { fontSize: 14, color: THEME.text_muted, marginTop: 4 },
  inputSection: { backgroundColor: THEME.bg_secondary, borderRadius: 15, padding: 15, marginBottom: 20, borderWidth: 1, borderColor: THEME.border },
  inputRow: { flexDirection: "row", justifyContent: "space-between", marginBottom: 15, alignItems: 'center' },
  inputCard: { flex: 1, marginRight: 10 },
  inputLabel: { color: THEME.text_muted, fontSize: 11, marginBottom: 5, fontWeight: "bold" },
  textInput: { backgroundColor: THEME.bg_input, color: "#fff", padding: 10, borderRadius: 8, fontSize: 16, borderWidth: 1, borderColor: THEME.border },
  calcButton: { backgroundColor: THEME.accent, padding: 12, borderRadius: 10, alignItems: 'center', justifyContent: 'center', flexDirection: 'row' },
  calcButtonText: { color: "#fff", fontWeight: "bold", fontSize: 16 },
  cardsRow: { flexDirection: "row", justifyContent: "space-between", marginBottom: 20 },
  card: { backgroundColor: THEME.bg_secondary, borderRadius: 12, padding: 10, width: (screenWidth - 40) / 3 - 8, borderWidth: 1, borderColor: THEME.border },
  cardHeader: { flexDirection: "row", alignItems: "center", marginBottom: 5 },
  cardTitle: { color: THEME.text_muted, fontSize: 8, marginLeft: 4, fontWeight: "bold" },
  cardValue: { color: "#fff", fontSize: 12, fontWeight: "bold" },
  sectionTitle: { color: "#fff", fontSize: 18, fontWeight: "bold", marginBottom: 15 },
  chartContainer: { marginBottom: 20, backgroundColor: THEME.bg_secondary, borderRadius: 15, padding: 15, borderWidth: 1, borderColor: THEME.border },
  chart: { marginVertical: 8, borderRadius: 16 },
  listSection: { backgroundColor: THEME.bg_secondary, borderRadius: 15, padding: 15, marginBottom: 20, borderWidth: 1, borderColor: THEME.border },
  listRow: { flexDirection: "row", alignItems: "center", justifyContent: "space-between", paddingVertical: 12, borderBottomWidth: 0.5, borderBottomColor: THEME.border },
  listAy: { color: "#fff", fontSize: 14, width: 60 },
  listValues: { flex: 1, flexDirection: "row", alignItems: "center" },
  listIade: { color: THEME.result, fontWeight: "bold", fontSize: 14 },
  listOran: { color: THEME.text_muted, fontSize: 11, marginLeft: 8 },
  statusBadge: { paddingHorizontal: 10, paddingVertical: 4, borderRadius: 12 },
  statusText: { color: "#fff", fontSize: 10, fontWeight: "bold" },
  resultBox: { backgroundColor: THEME.bg_input, padding: 15, borderRadius: 12, marginBottom: 20, alignItems: 'center', borderLeftWidth: 4, borderLeftColor: THEME.accent },
  resultValue: { color: THEME.result, fontSize: 20, fontWeight: 'bold' },
  resultValueTitle: { color: THEME.text_muted, fontSize: 12, marginBottom: 5 },
  currencySelect: { flex: 2, backgroundColor: THEME.bg_input, padding: 12, borderRadius: 8, alignItems: 'center' },
  currencySelectText: { color: '#fff', fontWeight: 'bold' },
  swapButton: { padding: 10 },
  modeButton: { padding: 12, borderBottomWidth: 0.5, borderBottomColor: THEME.border },
  modeButtonActive: { backgroundColor: THEME.bg_input },
  modeButtonText: { color: THEME.text_muted, fontSize: 14 },
  modeButtonTextActive: { color: THEME.accent, fontWeight: 'bold' },
  chipBtn: { paddingHorizontal: 10, paddingVertical: 6, borderRadius: 16, backgroundColor: THEME.bg_input, borderWidth: 1, borderColor: THEME.border },
  chipBtnActive: { backgroundColor: THEME.accent, borderColor: THEME.accent },
  chipText: { color: THEME.text_muted, fontSize: 12 },
  chipTextActive: { color: '#fff', fontWeight: 'bold' },
});

const compoundStyles = StyleSheet.create({
  modalOverlay: { flex: 1, backgroundColor: 'rgba(0,0,0,0.7)', justifyContent: 'flex-end' },
  modalBox:     { backgroundColor: THEME.bg_secondary, borderTopLeftRadius: 20, borderTopRightRadius: 20, padding: 20, maxHeight: '80%' },
  modalHeader:  { flexDirection: 'row', justifyContent: 'space-between', alignItems: 'center', marginBottom: 12 },
  modalTitle:   { color: '#fff', fontSize: 16, fontWeight: 'bold' },
  modalClose:   { color: THEME.text_muted, fontSize: 22, lineHeight: 24 },
  tableHeader:  { flexDirection: 'row', borderBottomWidth: 1, borderBottomColor: THEME.border, paddingBottom: 8, marginBottom: 4 },
  tableHeaderCell: { flex: 1, color: THEME.accent, fontWeight: 'bold', fontSize: 12, textAlign: 'center' },
  tableRow:     { flexDirection: 'row', paddingVertical: 8 },
  tableRowAlt:  { backgroundColor: THEME.bg_input },
  tableCell:    { flex: 1, color: '#fff', fontSize: 12, textAlign: 'center' },
});
