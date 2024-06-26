import React, { useEffect, useState } from 'react';
import { Text, View, TouchableOpacity, Image, FlatList, ActivityIndicator } from 'react-native';
import Tab from '../shared/tab';
import { getData } from '../shared/storage-utils';
import { shouldUseActivityState } from 'react-native-screens';
import { globalStyles } from '../shared/style-sheet';

function OneUser({navigation, route}){
    const {schedules, name, profileId} = route.params;
    const [data, setData] = useState([]);
    const [isLoading, setIsLoading] = useState(false);
    // const [data, setData] = useState([]);
    console.log(schedules);
    useEffect(() => {
        const printToken = async() => {
            const token = await getData('token');
            console.log(token)
        }
        printToken();
        console.log(profileId);

        fetchSlots();
    }, [navigation])
    const fetchSlots = async() => {
        setIsLoading(true);
        const token = await getData('token');
        console.log('awaiting...' + token);

        await fetch(url+'api/pills/slots', {
            method: 'GET',
            headers:{
                Accept: 'application/json',
                'Content-Type' : 'application/json',
                'Authorization' : 'Bearer '+token,
            },
        })
        .then(response => response.json() )
        .then(_data => {
            console.log(_data);
            const arr = [_data[0]['id'], _data[1]['id']];
            // setIds(arr);
            console.log(arr);
            setData(_data);
            setIsLoading(false);
        })
    
        .catch(e => {
            console.log('failed - ' + e);
        });
    }
    const createSchedule = async(slotId) =>{
        setIsLoading(true);
        const token = await getData('token');
        console.log('slotId: ' + slotId);
        await fetch(url+'api/pills/slots/'+slotId+'/schedules', {
            method: 'POST',
            headers:{
                Accept: 'application/json',
                'Content-Type' : 'application/json',
                'Authorization' : 'Bearer '+token,
            },
            body:JSON.stringify({
                "pillSchedules": [
                    {
                      "times": [  
                        {
                            "hour" : 0,
                            "minutes" : 0
                        }
                      ],
                      "profileId": profileId,
                      "quantity": 1
                    }
                  ]
            })
        })
        .then(response => response.status == 200 ? response.json() : console.log(response) )
        .then(_data => {
            // setIsLoading(false);
            fetchSlots();
        })
    
        .catch(e => {
            console.log('failed - ' + e);
        });

    }
    const renderSchedule = ({item}) => {
        const filteredSchedules = item.pillSchedules.filter(schedule => schedule.profile.id === profileId);
        console.log("FILTERED: " + filteredSchedules);
        console.log(filteredSchedules);
        const timesToSend = filteredSchedules[0]? filteredSchedules[0].times : [];
        return(
            <View style={{
                marginBottom: 12,
            }}>
                <Tab 
                    type='pill' 
                    title={item.pillName} 
                    // text={filteredSchedules[0]? filteredSchedules[0].times.length + " times" : '0 times'}
                    text={filteredSchedules[0]? filteredSchedules[0].times.length + " times" : 'Select to create schedule'}

                    onPress={async()=> {
                        if(filteredSchedules[0]){
                            navigation.navigate('OneSchedule', {profileId: item.profileId, scheduleId: filteredSchedules[0].id, slotId: item.id, times: timesToSend})
                        }
                        else{
                            await createSchedule(item.id);
                            
                            // navigation.navigate('OneSchedule', {profileId: item.profileId, scheduleId: foundProfile[0].id, slotId: item.id, times: timesToSend})

                        }
                    }
                        
                    }
                />
            </View>
        )
    }
    return(
        <View 
            style={{
                flex:1,
                alignItems: 'center',
            }}
        >
            <Text style={[globalStyles.title, globalStyles.headerCenter]}>
                {/* {schedules["pillSchedules"].pillSlotId} */}
                {name} 
            </Text>
            
                {
                    isLoading?(
                        <ActivityIndicator size={'large'}/> 
                    ) :
                    schedules == []
                        ? 
                            (<View>

                            </View> )
                        : 
                        (<FlatList
                            data ={data}
                            renderItem={renderSchedule}
                            keyExtractor={item => item.id }
                        />)

                }
                
                {/* <TouchableOpacity
                 style={{
                    height: 50,
                    width: 50,
                    // backgroundColor: '#00C780',
                    borderRadius: 50,
                    // alignSelf: 'center'
                }}
                onPress={() => navigation.navigate('OneSchedule', profileId)}
            >
                <Image 
                    source={require('../icons/plus-icon.png')}
                    style={{
                        height: 50,
                        width: 50,
                    }}    
                />
            </TouchableOpacity> */}
        </View>
    )
}

export default OneUser;