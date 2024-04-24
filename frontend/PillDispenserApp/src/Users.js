import React , {useEffect, useState} from 'react';
import { SafeAreaView, View, Text, Image, TextInput, TouchableOpacity, FlatList, ActivityIndicator  } from 'react-native';
import { NavigationAction } from '@react-navigation/native';
import url from '../shared/variables.js'
import Tab from '../shared/tab.js'
import SearchBar from '../shared/search-bar.js';
import { getData, storeData } from '../shared/storage-utils.js';
import AsyncStorage from '@react-native-async-storage/async-storage';
import { useLogin } from './context/LoginProvider.js';


function Users({navigation}){
    const {setIsLoggedIn} = useLogin();
    const [data, setData] = useState([]);
    const [loading, setLoading] = useState(false);
    const [loginChanged, setLoginChanged] = useState(false);
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
        // .then(response => console.log(response))
        .then(response => response.json()).then(_data => {
            // console.log(_data);
            setData(_data);
            console.log("success!");
            setLoading(false);
        })
        .catch(e => console.error(e))
    }
    useEffect(() => {
        // Update the document title using the browser API
        fetchUsers()
      }, []);
    
    const renderItem = ({item}) =>{
        return(
            <View style={{marginBottom:12}}>
                <Tab type='user' title={item.username} text='Tap to select'/>
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
            <Text>
                USERS
            </Text>
            <TouchableOpacity onPress={async() =>  {
                    await AsyncStorage.removeItem('token');
                    setIsLoggedIn(false);
                }}>
            <Text
                style={{color:'red'}}
            >
                Log Out
            </Text>
            </TouchableOpacity>
            

            <SearchBar/>
            {loading 
                ?   <ActivityIndicator
                        size={'large'}
                    />
                :  <FlatList
                        data={data}
                        renderItem={renderItem}
                        keyExtractor={item=>item.id}
                    />
            }
           
                

            <TouchableOpacity
                 style={{
                    height: 50,
                    width: 50,
                    // backgroundColor: '#00C780',
                    borderRadius: 50
                }}
                onPress={() => navigation.navigate('NewUser')}
            >
                <Image 
                    source={require('../icons/plus-icon.png')}
                    style={{
                        height: 50,
                        width: 50,
                    }}    
                />
            </TouchableOpacity>
        </View>
    )
}

export default Users;