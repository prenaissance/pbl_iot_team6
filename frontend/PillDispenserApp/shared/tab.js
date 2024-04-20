import React from 'react';
import { SafeAreaView, View, Text, Button, TextInput, TouchableOpacity, Image } from 'react-native';

function Tab(props){
    const tabIcons ={
        user: require('../icons/user-icon.png'),
    }
    switch(props.type){
        case 'user':
            iconPath = tabIcons.user;
        default:
            iconPath = tabIcons.user;
    }
    return(
        <TouchableOpacity>
            <View
                style={{
                    backgroundColor:'#e9e9e9',
                    height:75,
                    width:375,
                    alignContent: 'center',
                    flexDirection:'row',
                    alignItems: 'center',
                }}
            >
                <Image 
                    source={iconPath}
                    style={{
                        width:50,
                        height:50
                    }}
                />
                <View>
                    <Text>{props.title}</Text>
                    <Text>{props.text}</Text>
                </View>
                
            </View>
        </TouchableOpacity>
    )
}

export default Tab;