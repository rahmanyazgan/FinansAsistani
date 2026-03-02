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
} from "react-native";
import { StatusBar } from "expo-status-bar";
import { NavigationContainer } from "@react-navigation/native";
import { createBottomTabNavigator } from "@react-navigation/bottom-tabs";
import { BarChart } from "react-native-chart-kit";
import {
  Wallet,
  TrendingUp,
  FileText,
  ChevronRight,
  Info,
  RefreshCcw,
  Percent,
  Calculator
} from "lucide-react-native";
import {
  THEME,
  runCalculation,
  TaxResult,
  fetchExchangeRates,
  COMMON_CURRENCIES,
  PERCENT_MODES,
  calculatePercentage
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
            if (route.name === 'Kur') return <RefreshCcw color={color} size={size} />;
            if (route.name === 'Yüzde') return <Percent color={color} size={size} />;
            if (route.name === 'Vergi') return <Calculator color={color} size={size} />;
          },
        })}
      >
        <Tab.Screen name="Kur" component={CurrencyScreen} />
        <Tab.Screen name="Yüzde" component={PercentScreen} />
        <Tab.Screen name="Vergi" component={TaxScreen} />
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
  modeButtonTextActive: { color: THEME.accent, fontWeight: 'bold' }
});
