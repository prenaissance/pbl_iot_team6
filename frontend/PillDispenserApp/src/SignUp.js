import React, {useState}  from 'react';
import { SafeAreaView, View, Text, Button, TextInput, TouchableOpacity, Alert } from 'react-native';
import url from '../shared/variables';
import PillButton from '../shared/pill-button';
import {getData, storeData} from '../shared/storage-utils.js'
import RNBluetoothClassic, { BluetoothDevice } from 'react-native-bluetooth-classic';

function SignUp({navigation}){
    const [idRecieved, setIdRecieved] = useState(false);
    const [deviceId, setDeviceId] = useState("");
    const handlePair = async() => {
        const paired = await RNBluetoothClassic.getBondedDevices();
        // this.setState({paired});

        console.log(paired);
        
        // Alert.alert('PAIRED', JSON.stringify(paired));
        // Alert.alert(JSON.stringify(paired[0].name), JSON.stringify(paired[0].address));

        for (let i = 0; i < paired.length; i ++){
        // Alert.alert(String(device.name), String(device.address))
          if (String(paired[i].name) == "ESP32"){
            // Alert.alert('FOUND ESP32', String(paired[i].address));
            const connected = await paired[i].connect()
            // Alert.alert(String(connected), String(paired[i].address));
            const sentData = await paired[i].write("\"{\"reqCode\":2,\"payload\":{}}\"\n").then(
                async() => {
                    let id;
                    do 
                        id = await paired[i].read();
                    while (id === null)
                    let trimmedId = id.trim();
                    let slicedId = trimmedId.slice(1, -1);
                    // Alert.alert(String(id), 'wake up, im in your walls')
                    let realId = JSON.parse(slicedId).payload.deviceIdBase64;
                    setDeviceId(String(realId));
                    setIdRecieved(true);
                }
            );
            Alert.alert(String(sentData), String(paired[i].address));
            setIsLoading(false);
            }
        }
    }
    const handleSubmit = async() => {
        console.log(await getData('token'));
        const dataToSend = {
            username: username,
            password: password,
            deviceId: deviceId
        }
        console.log("awaiting...");
        await fetch(url+'api/authentication/register', {
            method: 'POST',
            headers: {
                Accept: 'application/json',
                'Content-Type' : 'application/json'
            },
            body: JSON.stringify(dataToSend),
        }).then(response => response.json()).then(
            data => {
                storeData('token', data['token']);
                console.log('success!');
                Alert.alert('Success!', 'User created successfully');
            }
        ).catch(error => {
            console.error(error);
        })
    }
    const [username, setUsername] = useState('');
    const [password, setPassword] = useState('');
    return (
        <SafeAreaView style=
        {{
            flex: 1,
            alignItems: 'center',
        }}>
            <TextInput
                placeholder='Username'
                onChangeText={value => setUsername(value)}
                style={{
                    backgroundColor: '#E9E9E9', 
                    height: 73,
                    width: 350,
                    borderRadius: 10, 
                    marginTop: 126, 
                    fontSize: 24,
                }}
            />
            <TextInput
                placeholder='Password'
                onChangeText={value => setPassword(value)}
                style={{
                    backgroundColor: '#E9E9E9', 
                    height: 73,
                    width: 350,
                    borderRadius: 10, 
                    marginTop: 16, 
                    fontSize: 24,
                }}
            />
            <TouchableOpacity  onPress={() => !idRecieved? handlePair(): {}}>
                <Text style={{color:'green'}}>{!idRecieved? ('Connect to device') : String(deviceId)}</Text>
            </TouchableOpacity>
            <Text style={{
                marginTop:15
            }}>Already have an account?</Text>
            <TouchableOpacity onPress={() => navigation.navigate("Login")}>
                <Text style={{color:'#40BAFF'}}>Login</Text>
                </TouchableOpacity>

            <View style={[{marginTop: 265}, !idRecieved? {opacity: .2} : {opacity: 1}]}>
                <PillButton
                    text='Sign Up'
                    onPress={idRecieved? () => handleSubmit(): null}
                    
                />
            </View>
            
        </SafeAreaView>
    )
}

export default SignUp;