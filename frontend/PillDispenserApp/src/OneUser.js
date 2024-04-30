import React, { useEffect, useState } from 'react';
import { Text, View, TouchableOpacity, Image, FlatList } from 'react-native';
import Tab from '../shared/tab';
import { getData } from '../shared/storage-utils';

function OneUser({navigation, route}){
    const {schedules, name, profileId} = route.params;
    // const [data, setData] = useState([]);
    console.log(schedules);
    useEffect(() => {
        const printToken = async() => {
            const token = await getData('token');
            console.log(token)
        }
        printToken();
        console.log(profileId);
    }, [])
    const renderSchedule = ({item}) => {
        return(
            <Tab type='user' title={item.id} text={item.owner}/>
        )
    }
    return(
        <View 
            style={{
                flex:1,
            }}
        >
            <Text>
                {/* {schedules["pillSchedules"].pillSlotId} */}
                {name} 
            </Text>
            
                {
                    schedules == []
                        ? 
                            <View>

                            </View> 
                        : 
                        <FlatList
                            data ={schedules}
                            renderItem={renderSchedule}
                            keyExtractor={item => item.id }
                        />

                }
                
                <TouchableOpacity
                 style={{
                    height: 50,
                    width: 50,
                    // backgroundColor: '#00C780',
                    borderRadius: 50,
                    // alignSelf: 'center'
                }}
                onPress={() => navigation.navigate('NewSchedule', profileId)}
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

export default OneUser;