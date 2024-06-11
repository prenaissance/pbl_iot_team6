import React, {useState} from 'react';
import { SafeAreaView, View, Text, Image, TextInput, TouchableOpacity, FlatList, Alert } from 'react-native';
import PillButton from '../shared/pill-button';
import url from '../shared/variables.js';
import {getData, storeData} from '../shared/storage-utils.js';
import { globalStyles } from '../shared/style-sheet.js';
import RNBluetoothClassic, { BluetoothDevice } from 'react-native-bluetooth-classic';

function NewUser({navigation}){
   
    const [name, setName] = useState('');
    const [rfid, setRfid] = useState('');

    const handleSubmit = async() => {
        const token = await getData('token');
        const dataToSend = {
            username: name
        };
        console.log(dataToSend);
        await fetch(url + 'api/profiles', {
            method: 'POST',
            headers:{
                Accept: 'application/json',
                'Content-Type' : 'application/json',
                'Authorization' : 'Bearer '+token,
            },
            body: JSON.stringify(dataToSend),
        })
        // .then(response => {
        //     console.log(response);
        // })
        .then(response => response.json()).then(data => {
            
            // navigation.navigate('Users');
            Alert.alert('RFID', 'Please present RFID!');
            const handleRfid = async() =>{
                try {            
                    const paired = await RNBluetoothClassic.getBondedDevices();
            
                    for (let i = 0; i < paired.length; i ++){
                      if (String(paired[i].name) == "ESP32"){
                        const connected = await paired[i].connect()
                        const sentData = await paired[i].write("\"{\"reqCode\":1,\"payload\":{}}\"\n").then(
                            async() => {
                                let response;
                                do 
                                    response = await paired[i].read();
                                while (response === null);

                                let trimmedResponse = response.trim();
                                let slicedResponse = trimmedResponse.slice(1, -1);
                                let realResponse;
                                try{
                                    realResponse = JSON.parse(slicedResponse).payload.rfid;

                                } catch(e){
                                    Alert.alert(e);
                                }

                                setRfid(String(realResponse));
                                const dataToSend = {
                                    profileId : data.id,
                                    rfid: String(realResponse), 
                                }
                                await fetch(url + 'api/profiles/rfid', {
                                    method: 'PATCH',
                                    headers:{
                                        Accept: 'application/json',
                                        'Content-Type' : 'application/json',
                                        'Authorization' : 'Bearer '+token,
                                    },
                                    body: JSON.stringify(dataToSend),
                                }).then(response => navigation.navigate('Users'));
                            });

                            }
                        }
                  } catch (err) {
                      // Error if Bluetooth is not enabled
                      // Or there are any issues requesting paired devices
                  }
            }
            handleRfid();
        })
        .catch(e => {
            console.error(e);
        })
    } 
    return(
        <View
            style={{
                flex: 1,
            }}
        >
            <Text style={[globalStyles.title, globalStyles.header]}>
                New User
            </Text>
            <View
                style={{
                    alignItems: 'center',
                }}
            >
                <Image source={require('../icons/user-icon.png')} style={{marginVertical:20}}/>
                <TextInput
                    placeholder='name'
                    onChangeText={value => setName(value)}
                    style={{
                        backgroundColor: '#E9E9E9', 
                        height: 50,
                        width: 350,
                        borderRadius: 10, 
                        marginTop: 16, 
                        fontSize: 18,
                        marginVertical: 20
                    }}
                />
                <View style={{marginTop:260}}>
                    <PillButton
                        text='Create User'
                        onPress={()=> handleSubmit()}
                    />
                </View>
               <Text style={{color:"green"}}>{rfid}</Text>
            </View>
           
        </View>
    )
}

export default NewUser;