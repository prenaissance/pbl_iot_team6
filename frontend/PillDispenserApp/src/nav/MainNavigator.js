import React, { useContext, useEffect, useState } from 'react';
import { NavigationContainer } from '@react-navigation/native';
import StartUpStack from './StartUpStack';
import MyTabs from './MyTabs';
import { useLogin } from '../context/LoginProvider';
import { View, Alert } from 'react-native';
import { getData } from '../../shared/storage-utils';
import SplashScreen from '../SplashScreen';
import RNBluetoothClassic, { BluetoothDevice } from 'react-native-bluetooth-classic';

function MainNavigator(){
  const [isLoading, setIsLoading] = useState(false);
  const [isAvailable, setIsAvailable] = useState(false);
  const [isEnabled, setIsEnabled] = useState(false);
  const [paired, setPaired] = useState([]);

  useEffect(() => {

    const connectBluetooth = async() => {
      try {
        setIsLoading(true);

        const paired = await RNBluetoothClassic.getBondedDevices();

        console.log(paired);
        
        Alert.alert('PAIRED', JSON.stringify(paired));

        for (let i = 0; i < paired.length; i ++){
          if (String(paired[i].name) == "ESP32"){
            Alert.alert('FOUND ESP32', String(paired[i].address));
            const connected = await paired[i].connect()
            Alert.alert(String(connected), String(paired[i].address));
            const sentData = await paired[i].write("\"{\"reqCode\":3,\"payload\":{\"ssid\":\"Redmi Note 9 Pro\",\"pass\":\"12345768\"}}\"\n");
            Alert.alert(String(sentData), String(paired[i].address));
            setIsLoading(false);
          }
        }
      } catch (err) {
          // Error if Bluetooth is not enabled
          // Or there are any issues requesting paired devices
      }
    }
    const checkLogIn = async () => {
      setIsLoading(true);
      
      if (await getData('token') != null){
        setIsLoggedIn(true);
      } 
      setIsLoading(false);
    }
    
    checkLogIn();   
    // connectBluetooth(); 
  }, [])
    const {isLoggedIn, setIsLoggedIn} = useLogin();
    return(
      
          isLoading? (
            <SplashScreen/>
          ) : 
          !isLoggedIn ? (
            <StartUpStack/>
          ) : (
            <MyTabs/>
          )

        
    )
}

export default MainNavigator;