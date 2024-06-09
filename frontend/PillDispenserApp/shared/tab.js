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
                    flexDirection:'row',

                    alignContent: 'center',
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
                <View style={{ flex: 1, flexDirection: 'row', justifyContent: 'space-between', alignItems: 'center' }}>
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
                    {props.onDelete? (
                        
                        <TouchableOpacity style={{marginRight: 20}} onPress={props.onDelete}>
                            <Text style={{color:'red'}}>
                                Delete
                            </Text>
                        </TouchableOpacity>
                    ): null}
                    
                </View>
                
            </View>
        </TouchableOpacity>
    )
}

export default Tab;