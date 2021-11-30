EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "PETI Prototype Backplane Daughterboard"
Date "2021-08-24"
Rev ""
Comp "Arcana Labs"
Comment1 "Not for resale model"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:Battery BT1
U 1 1 61256E0E
P 5650 2200
F 0 "BT1" H 5758 2246 50  0000 L CNN
F 1 "Battery 2xAA Series" H 5758 2155 50  0000 L CNN
F 2 "Battery:BatteryHolder_Keystone_2462_2xAA" V 5650 2260 50  0001 C CNN
F 3 "~" V 5650 2260 50  0001 C CNN
	1    5650 2200
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_02x10_Odd_Even J1
U 1 1 61259BA2
P 5000 4700
F 0 "J1" H 5050 5317 50  0000 C CNN
F 1 "BOOSTER_LEFT" H 5050 5226 50  0000 C CNN
F 2 "Connector_PinHeader_2.00mm:PinHeader_2x10_P2.00mm_Vertical" H 5000 4700 50  0001 C CNN
F 3 "~" H 5000 4700 50  0001 C CNN
	1    5000 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 2200 5200 2200
Wire Wire Line
	4350 2200 4350 4300
Wire Wire Line
	4350 4300 4800 4300
$Comp
L Switch:SW_DIP_x02 SW1
U 1 1 6125F5A6
P 4900 2200
F 0 "SW1" H 4900 2567 50  0000 C CNN
F 1 "SW_DIP_x01 CUT LEADS" H 4900 2476 50  0000 C CNN
F 2 "Button_Switch_THT:SW_DIP_SPSTx02_Slide_6.7x6.64mm_W7.62mm_P2.54mm_LowProfile" H 4900 2200 50  0001 C CNN
F 3 "~" H 4900 2200 50  0001 C CNN
	1    4900 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4600 2200 4350 2200
NoConn ~ 5300 4300
Wire Wire Line
	8900 4350 9550 4350
NoConn ~ 4800 4400
NoConn ~ 4800 4700
NoConn ~ 4800 4800
NoConn ~ 4800 5000
NoConn ~ 8900 4450
NoConn ~ 8900 4550
NoConn ~ 8900 4650
NoConn ~ 8900 4950
NoConn ~ 8900 5050
NoConn ~ 8900 5150
NoConn ~ 4800 4600
NoConn ~ 4800 5100
$Comp
L Device:R R2
U 1 1 6126D5AD
P 5750 5350
F 0 "R2" V 5957 5350 50  0000 C CNN
F 1 "100 O" V 5866 5350 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 5680 5350 50  0001 C CNN
F 3 "~" H 5750 5350 50  0001 C CNN
	1    5750 5350
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R3
U 1 1 6126DBB1
P 5750 5700
F 0 "R3" V 5543 5700 50  0000 C CNN
F 1 "100" V 5634 5700 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0204_L3.6mm_D1.6mm_P5.08mm_Horizontal" V 5680 5700 50  0001 C CNN
F 3 "~" H 5750 5700 50  0001 C CNN
	1    5750 5700
	0    1    1    0   
$EndComp
$Comp
L Device:Speaker LS1
U 1 1 6126F129
P 7250 4600
F 0 "LS1" V 7259 4320 50  0000 R CNN
F 1 "Device Buzzer" V 7168 4320 50  0000 R CNN
F 2 "Buzzer_Beeper:Buzzer_12x9.5RM7.6" H 7250 4400 50  0001 C CNN
F 3 "~" H 7240 4550 50  0001 C CNN
	1    7250 4600
	0    1    1    0   
$EndComp
$Comp
L Device:LED D1
U 1 1 6126FF09
P 6300 5350
F 0 "D1" H 6293 5095 50  0000 C CNN
F 1 "LED_BATT" H 6293 5186 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 6300 5350 50  0001 C CNN
F 3 "~" H 6300 5350 50  0001 C CNN
	1    6300 5350
	-1   0    0    1   
$EndComp
$Comp
L Device:LED D2
U 1 1 612708B6
P 6300 5700
F 0 "D2" H 6293 5445 50  0000 C CNN
F 1 "LED_ALERT" H 6293 5536 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 6300 5700 50  0001 C CNN
F 3 "~" H 6300 5700 50  0001 C CNN
	1    6300 5700
	-1   0    0    1   
$EndComp
Wire Wire Line
	5900 5700 6150 5700
Wire Wire Line
	7050 5700 7050 5350
Wire Wire Line
	6450 5700 7050 5700
Wire Wire Line
	5500 5350 5600 5350
Wire Wire Line
	5450 5700 5600 5700
Wire Wire Line
	5900 5350 6150 5350
Wire Wire Line
	4800 4900 4400 4900
Wire Wire Line
	4400 4900 4400 5550
Wire Wire Line
	4400 5550 4250 5550
Wire Wire Line
	5300 5200 5300 5650
Wire Wire Line
	5300 5650 4250 5650
Wire Wire Line
	9550 6100 4250 6100
Wire Wire Line
	4250 6100 4250 5850
Wire Wire Line
	4250 5750 4450 5750
Wire Wire Line
	4450 5750 4450 5850
Wire Wire Line
	4450 5850 9300 5850
Wire Wire Line
	9300 5850 9300 4850
Wire Wire Line
	8900 4850 9300 4850
Wire Wire Line
	8900 4750 9550 4750
Wire Wire Line
	9550 4750 9550 6100
$Comp
L Switch:SW_DIP_x02 SW2
U 1 1 6129185B
P 8650 3750
F 0 "SW2" H 8650 4117 50  0000 C CNN
F 1 "SW_DIP_x02" H 8650 4026 50  0000 C CNN
F 2 "Button_Switch_THT:SW_DIP_SPSTx02_Slide_6.7x6.64mm_W7.62mm_P2.54mm_LowProfile" H 8650 3750 50  0001 C CNN
F 3 "~" H 8650 3750 50  0001 C CNN
	1    8650 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	8400 4350 8000 4350
Wire Wire Line
	8000 4350 8000 3750
Wire Wire Line
	8000 3750 8350 3750
Wire Wire Line
	8400 4450 7900 4450
Wire Wire Line
	7900 4450 7900 3650
Wire Wire Line
	7900 3650 8350 3650
Wire Wire Line
	8950 3650 9550 3650
Connection ~ 9550 3650
Wire Wire Line
	9550 3650 9550 2200
Wire Wire Line
	8950 3750 9550 3750
Wire Wire Line
	9550 3650 9550 3750
Connection ~ 9550 3750
Wire Wire Line
	9550 3750 9550 4350
$Comp
L Connector_Generic:Conn_02x10_Odd_Even J2
U 1 1 6125A876
P 8600 4750
F 0 "J2" H 8650 5367 50  0000 C CNN
F 1 "BOOSTER_RIGHT" H 8650 5276 50  0000 C CNN
F 2 "Connector_PinHeader_2.00mm:PinHeader_2x10_P2.00mm_Vertical" H 8650 5275 50  0001 C CNN
F 3 "~" H 8600 4750 50  0001 C CNN
	1    8600 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 2200 9550 2200
$Comp
L Connector:Conn_01x04_Female J3
U 1 1 6127399A
P 4050 5750
F 0 "J3" H 4078 5726 50  0000 L CNN
F 1 "EXP_SPI" H 4078 5635 50  0000 L CNN
F 2 "Connector_PinHeader_2.00mm:PinHeader_1x04_P2.00mm_Horizontal" H 4050 5750 50  0001 C CNN
F 3 "~" H 4050 5750 50  0001 C CNN
	1    4050 5750
	-1   0    0    1   
$EndComp
Wire Wire Line
	5450 5700 5450 5100
Wire Wire Line
	5450 5100 5300 5100
Wire Wire Line
	5500 5000 5300 5000
Wire Wire Line
	5500 5000 5500 5350
Wire Wire Line
	5300 4400 6600 4400
Wire Wire Line
	8400 4600 8400 4650
Wire Wire Line
	6600 5350 6600 4400
Wire Wire Line
	6450 5350 6600 5350
Connection ~ 6600 5350
Wire Wire Line
	6600 5350 7050 5350
Wire Wire Line
	8400 4600 7450 4600
Wire Wire Line
	7450 4600 7450 4400
Wire Wire Line
	7450 4400 7250 4400
Connection ~ 6600 4400
Wire Wire Line
	6600 4400 7150 4400
$EndSCHEMATC
