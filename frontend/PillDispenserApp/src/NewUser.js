import React, {useState} from 'react';
import { SafeAreaView, View, Text, Image, TextInput, TouchableOpacity, FlatList } from 'react-native';
import PillButton from '../shared/pill-button';
import url from '../shared/variables.js';
import {getData, storeData} from '../shared/storage-utils.js';
import { globalStyles } from '../shared/style-sheet.js';
function NewUser({navigation}){
   
    const [name, setName] = useState('');
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
            navigation.navigate('Users');
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
               
            </View>
           
        </View>
    )
}

export default NewUser;