// ViewController.swift
// Wilson Golf

import Foundation
import UIKit
import CoreBluetooth

var txCharacteristic : CBCharacteristic?
var rxCharacteristic : CBCharacteristic?
var blePeripheral : CBPeripheral?
var characteristicASCIIValue = NSString()

class ViewController: UIViewController, CBCentralManagerDelegate, CBPeripheralDelegate {
    //Data
    var centeralManager : CBCentralManager!
    var RSSIs = [NSNumber]()
    var data = NSMutableData()
    var writeData: String = ""
    var peripherals = [CBPeripheral]()
    var characteristicValue = [CBUUID: NSData]()
    var timer = Timer()
    var characteristic = [String : CBCharacteristic]()


    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral,advertisementData: [String : Any], rssi RSSI: NSNumber) {
            cancelScan()
            self.peripherals.append(peripheral)
            self.RSSIs.append(RSSI)
            peripheral.delegate = self
            peripheral.discoverServices([BLEService_UUID])
            if blePeripheral == nil {
                print("Found new peripheral devices with services")
                print("Peripheral name: \(String(describing: peripheral.name))")
                print("************")
                print("Advertisemnt Data : \(advertisementData)")
                blePeripheral = peripheral
            }
    }

    func centeralManagerDidUpdateState(_ central: CBCentralManager) {

    }

    override func viewDidLoad() {
        super.viewDidLoad()
    }

    @objc func cancelScan() {
        self.centralManager?.stopScan()
        print("Scan Stopped")
        print("Number of Peripherals Found: peripherals.count)")
    }

    @IBAction func ClickedButton(_ sender: Any) {
        self.peripherals = []
        self.RSSIs = []
        startScan()
    }

    func startScan() {
        print("Now Scanning...")
        self.timer.invalidate()
        centeralManager?.scanForPeripherals(withServices: [BLEService_UUID] , options: [CBCentralManagerScanOptionAllowDuplicatesKey:false])
        Timer.scheduledTime(timeInterval: 17, target: self, selectior: #selector(self.cancelScan), userInfo: nil, repeats: false)        
    }
}
