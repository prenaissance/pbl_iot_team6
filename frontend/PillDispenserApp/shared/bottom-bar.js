import React from 'react';
import { SafeAreaView, View, Text, Button, TextInput, TouchableOpacity, Image } from 'react-native';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
const Tab = createBottomTabNavigator();

function BottomBar(){
    
    return(
        <View
            style={{
                flexDirection:'row',
                height:75,
                width:'100%',
                justifyContent:'space-around',
                alignItems:'center',
            }}
        >
            <TouchableOpacity>
                <Image
                    source={require('../icons/users-nav.png')}
                    style={{
                        width:34,
                        height:34
                    }}
                />
            </TouchableOpacity>
            <TouchableOpacity>
                <Image
                    source={require('../icons/containers-nav.png')}
                    style={{
                        width:34,
                        height:34
                    }}
                />
            </TouchableOpacity> 
            <TouchableOpacity>
                <Image
                    source={require('../icons/notifs-nav.png')}
                    style={{
                        width:34,
                        height:34
                    }}
                />
            </TouchableOpacity>
        </View>
    )
}

export default BottomBar;