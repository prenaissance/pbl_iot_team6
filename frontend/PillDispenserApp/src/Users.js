import React from 'react';
import { SafeAreaView, View, Text, Image, TextInput, TouchableOpacity, FlatList } from 'react-native';
import Tab from '../shared/tab.js'
import SearchBar from '../shared/search-bar.js';
import BottomBar from '../shared/bottom-bar.js';
function Users(){

    data = [
        {
            name: 'Alex',
            id: '1'
        },
        {
            name: 'Artiom',
            id: '2'
        },
        {
            name: 'Max',
            id: '3'
        },
        {
            name: 'Misha',
            id: '4'
        }
    ];

    const renderItem = ({item}) =>{
        return(
            <Tab type='user' title={item.name} text='Press to see details'/>
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

            <SearchBar/>
            <FlatList
                data={data}
                renderItem={renderItem}
                keyExtractor={item=>item.id}
            />
                

            <TouchableOpacity
                 style={{
                    height: 50,
                    width: 50,
                    // backgroundColor: '#00C780',
                    borderRadius: 50
                }}
            >
                <Image 
                    source={require('../icons/plus-icon.png')}
                    style={{
                        height: 50,
                        width: 50,
                    }}    
                />
                {/* <View 
                    style={{
                        height: 50,
                        width: 50,
                        backgroundColor: '#00C780',
                        borderRadius: 50
                    }}
                >
                </View> */}
            </TouchableOpacity>
                <BottomBar/>
        </View>
    )
}

export default Users;