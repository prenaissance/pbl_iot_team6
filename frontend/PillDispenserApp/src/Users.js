import React , {useEffect, useState, useCallback} from 'react';
import { SafeAreaView, View, Text, Image, TextInput, TouchableOpacity, FlatList, ActivityIndicator  } from 'react-native';
import { NavigationAction, useFocusEffect } from '@react-navigation/native';
import url from '../shared/variables.js'
import Tab from '../shared/tab.js'
import SearchBar from '../shared/search-bar.js';
import { getData, storeData } from '../shared/storage-utils.js';
import AsyncStorage from '@react-native-async-storage/async-storage';
import { useLogin } from './context/LoginProvider.js';
import { globalStyles } from '../shared/style-sheet.js';


function Users({navigation}){
    const {setIsLoggedIn} = useLogin();
    const [data, setData] = useState([]);
    const [loading, setLoading] = useState(false);
    const fetchUsers = async() =>{
        const token = await getData('token');
        console.log('awaiting...');
        setLoading(true);
        await fetch(url+'api/profiles',{
            method: 'GET',
            headers:{
                Accept: 'application/json',
                'Content-Type' : 'application/json',
                'Authorization' : 'Bearer '+token,
            },
        })  
        .then(response => response.json()).then(_data => {
            setData(_data);
            console.log("success!");
            setLoading(false);
        })
        .catch(e => console.error(e))
    }
    useFocusEffect(
        useCallback(() => {
            fetchUsers();
        }, [])
    )
    
    const handleDelete = async(id) => {
        const token = await getData('token');
        setLoading(true);
        console.log('awaiting delete...');
        await fetch(url+'api/profiles/'+id,{
            method: 'DELETE',
            headers:{
                Accept: 'application/json',
                'Content-Type' : 'application/json',
                'Authorization' : 'Bearer '+token,
            },
        })  
        .then(response => response.status == '200'? response.json(): console.log(response))
        .then(_data => {
            console.log("delete success!");
            fetchUsers();
            // setLoading(false);
        })
        .catch(e => console.error(e))
    }
    const renderItem = ({item}) =>{
        return(
            <View style={{marginBottom:12}}>
                <Tab type='user' title={item.username} text='Tap to select' onDelete={() => handleDelete(item.id)}
                    onPress={() => navigation.navigate('OneUser', {schedules: item.pillSchedules, name:item.username, profileId: item.id})}
                />
            </View>
        )
    }
    return(
        <View 
            style={{
                flex: 1,
                flexDirection: 'column',
            }}
        >
            
            <View style={{
                // flex:1,
                flexDirection: 'row',
                justifyContent: 'space-between'
            }}>
                <Text style={[globalStyles.title, globalStyles.header]}>
                    USERS
                </Text>
                <TouchableOpacity onPress={async() =>  {
                        await AsyncStorage.removeItem('token');
                        setIsLoggedIn(false);
                    }}
                    style={{
                        // marginLeft:32,
                        marginTop:50,
                        marginRight:32  
                    }}
                    >
                <Text
                    style={{color:'red'}}
                >
                    Log Out
                </Text>
                </TouchableOpacity>
            </View>
            
            

            <View style={{
                flex: 1,
                flexDirection: 'column',
                justifyContent: 'center',
                alignContent: 'center',
                alignItems: 'center'
            }}>

            {loading 
                ?   <ActivityIndicator
                        size={'large'}
                    />
                :  <FlatList
                        data={data}
                        renderItem={renderItem}
                        keyExtractor={item=>item.id}
                        style={{paddingVertical: 15}}
                    />
            }

            <TouchableOpacity
                 style={{
                    height: 50,
                    width: 50,
                    // backgroundColor: '#00C780',
                    borderRadius: 50,
                    marginVertical:15
                    // alignSelf: 'center'
                }}
                onPress={() => navigation.navigate('NewUser')}
            >
                <Image 
                    source={require('../icons/plus-icon.png')}
                    style={{
                        height: 50,
                        width: 50,
                        // marginBottom: 
                    }}    
                />
            </TouchableOpacity>
            </View>
           
                

            
        </View>
    )
}

export default Users;