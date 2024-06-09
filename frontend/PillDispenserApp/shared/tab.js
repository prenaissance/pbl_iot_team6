import React from 'react';
import { SafeAreaView, View, Text, Button, TextInput, TouchableOpacity, Image } from 'react-native';

function Tab(props){
    const tabIcons ={
        user: require('../icons/user-icon.png'),
        clock: require('../icons/clock-icon.png'),
        pill: require('../icons/pill-icon.png'),
    }
    switch(props.type){
        case 'user':
            iconPath = tabIcons.user;
            break;
        case 'clock':
            iconPath = tabIcons.clock;
            break;
        case 'pill':
            iconPath = tabIcons.pill;
            break;
        default:
            iconPath = tabIcons.pill;
            break;
    }
    return(
        <TouchableOpacity
            style={{
                width:375,
            }}
            onPress={props.onPress}
        >
            <View
                style={{
                    backgroundColor:'#e9e9e9',
                    height:75,
                    width:375,
                    alignContent: 'center',
                    flexDirection:'row',
                    alignItems: 'center',
                    borderRadius: 5,
                }}
            >
                <Image 
                    source={iconPath}
                    style={{
                        width:50,
                        height:50,
                        marginRight: 10,
                        marginLeft: 10,
                    }}
                />
                <View>
                    <Text
                        style={{
                            color: '#474747',
                            fontSize: 20,
                            fontWeight: 'bold',
                        }}
                    >{props.title}</Text>
                    <Text>{props.text}</Text>
                </View>
                
            </View>
        </TouchableOpacity>
    )
}

export default Tab;