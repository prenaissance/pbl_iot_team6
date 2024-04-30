import React, {useState} from 'react';
import { SafeAreaView, View, Text, Image, TextInput, TouchableOpacity, FlatList } from 'react-native';
import PillButton from '../shared/pill-button';
import url from '../shared/variables.js';
import {getData, storeData} from '../shared/storage-utils.js';

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
            <Text>
                New User
            </Text>
            <View
                style={{
                    alignItems: 'center',
                }}
            >
                <Image source={require('../icons/user-icon.png')}/>
                <TextInput
                    placeholder='name'
                    onChangeText={value => setName(value)}
                    style={{
                        backgroundColor: '#e9e9e9',
                        width: 400,
                        height: 40,
                        borderRadius: 50,
                    }}
                />
                <PillButton
                    text='Create User'
                    onPress={()=> handleSubmit()}
                />
            </View>
           
        </View>
    )
}

export default NewUser;