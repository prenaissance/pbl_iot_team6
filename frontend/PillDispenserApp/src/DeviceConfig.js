import React, {useState} from "react";
import { View, Text, TouchableOpacity, TextInput, Alert } from "react-native";
import PillButton from "../shared/pill-button";
import { globalStyles } from "../shared/style-sheet";
import RNBluetoothClassic, { BluetoothDevice } from 'react-native-bluetooth-classic';


function DeviceConfig(){
    const [wifiName, setWifiName] = useState("");
    const [wifiPass, setWifiPass] = useState("");


    const handleWifi = async(name, pass) => {
        try {
            // setIsLoading(true);
    
            const paired = await RNBluetoothClassic.getBondedDevices();
    
            // console.log(paired);
            
            // Alert.alert('PAIRED', "\"{\"reqCode\":3,\"payload\":{\"ssid\":\""+name+"\",\"pass\":\""+pass+"\"}}\"\n");
    
            for (let i = 0; i < paired.length; i ++){
              if (String(paired[i].name) == "ESP32"){
                const connected = await paired[i].connect()
                const sentData = await paired[i].write("\"{\"reqCode\":3,\"payload\":{\"ssid\":\""+name+"\",\"pass\":\""+pass+"\"}}\"\n");
                // // Alert.alert('Success!', String(paired[i].address));
                // setIsLoading(false);
              }
            }
          } catch (err) {
              // Error if Bluetooth is not enabled
              // Or there are any issues requesting paired devices
          }
    }

    const handleUpdate = async() => {
        try {
            const paired = await RNBluetoothClassic.getBondedDevices();            
            // Alert.alert('PAIRED', "\"{\"reqCode\":4,\"payload\":{}}\"\n");
            //"\"{\"reqCode\":1,\"payload\":{}}\"\n"

            for (let i = 0; i < paired.length; i ++){
              if (String(paired[i].name) == "ESP32"){
                // Alert.alert('FOUND ESP32', String(paired[i].address));
                const connected = await paired[i].connect()
                // Alert.alert(String(connected), String(paired[i].address));
                const sentData = await paired[i].write("\"{\"reqCode\":4,\"payload\":{}}\"\n");
                // Alert.alert(String(sentData), String(paired[i].address));
                // setIsLoading(false);
              }
            }
          } catch (err) {
              // Error if Bluetooth is not enabled
              // Or there are any issues requesting paired devices
          }
    }
    return(
        <View 
            style={{
                flex: 1,
                flexDirection: "column",
                alignItems: "center",
                justifyContent: "space-around"
            }}
        >
            <Text style={[globalStyles.title, globalStyles.headerCenter]}>Device Config</Text>
            <View>
                <TextInput placeholder="wifi name" onChangeText={value => setWifiName(value)}/>
                <TextInput placeholder="wifi password" onChangeText={value => setWifiPass(value)}/>
                <PillButton onPress ={() => {handleWifi(wifiName, wifiPass)}} text="Set Wifi"/>
            </View>
            <View
                style={{backgroundColor: "#474747", height: 1, width: "80%", marginBottom: 10}}
            ></View>

            <PillButton onPress={() => handleUpdate()} text ="Update Data"/>
        </View>
    )
}

export default DeviceConfig;