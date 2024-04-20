import React, {useState}  from 'react';
import { SafeAreaView, View, Text, Button, TextInput, TouchableOpacity } from 'react-native';
import url from '../shared/variables';

function LogIn(){
    const handleSubmit = async() => {
        const dataToSend = {
            username: username,
            password: password,
            deviceId: "22ad34c6-2ff2-4dcf-a826-1994da215c8b"
        }

        await fetch(url+'api/authentication/register', {
            method: 'POST',
            headers: {
                Accept: 'application/json',
                'Content-Type' : 'application/json'
            },
            body: JSON.stringify(dataToSend),
        }).then(response => {
            console.log(response)
        }).catch(error => {
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
            <Text style={{
                marginTop:15
            }}>Don't have an account? Sign Up</Text>

            <TouchableOpacity
                style={{
                    width:300,
                    height: 73, 
                    borderRadius: 10,
                    backgroundColor: '#00C780',
                    color: 'white',
                    justifyContent: 'center',
                    alignContent: 'center',
                    marginTop: 265
                }}
                onPress={() => handleSubmit()}
            >
                <Text style={{
                    textAlign: 'center',
                    color: 'white',
                    fontSize: 25
                }}>Log In</Text>
            </TouchableOpacity>
        </SafeAreaView>
    )
}

export default LogIn;