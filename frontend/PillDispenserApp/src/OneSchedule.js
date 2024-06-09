import React, { useEffect, useState } from 'react';
import { ActivityIndicator, FlatList, Text, TextInput, View } from 'react-native';
import PillButton from '../shared/pill-button';
import url from '../shared/variables'
import { getData } from '../shared/storage-utils';
import Tab from '../shared/tab';
import { globalStyles } from '../shared/style-sheet';
function OneSchedule({navigation, route}){
    const [loading, setLoading] = useState(false);
    const [hours, setHours] = useState('');
    const [minutes, setMinutes] = useState('');
    const [quantity, setQuanity] = useState('');
    const [scheduleData, setScheduleData] = useState({});

    const { profileId, scheduleId, slotId, times } = route.params; //times + slotId

    function findPillScheduleById(data, scheduleId) {
        for (const item of data) {
          const pillSchedule = item.pillSchedules.find(schedule => schedule.id === scheduleId);
          if (pillSchedule) {
            return pillSchedule;
          }
        }
        return null; // Return null if no matching schedule is found
      }
    const fetchSlots = async() => {
        setLoading(true);
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
            const filteredData = findPillScheduleById(_data, scheduleId);
            setScheduleData(filteredData);
            setQuanity(filteredData.quantity);
            setLoading(false);
        })
    
        .catch(e => {
            console.log('failed - ' + e.data);
            setLoading(false);
        });
    }

    const createSchedule = async() => {
        const token = await getData('token');
        console.log('awaiting...' + token);
        const newTime = hours & minutes ? [...scheduleData.times, {'hour' : hours, 'minutes' : minutes}]: scheduleData.times;
        console.log(JSON.stringify({
            'id' : scheduleId,
            'times': newTime,
                'quantity' : quantity
        }));
        fetch(url + 'api/pills/slots/schedules', {
            method: 'PUT',
            headers:{
                Accept: 'application/json',
                'Content-Type' : 'application/json',
                'Authorization' : 'Bearer '+token,
            },
            body:JSON.stringify({
                'id' : scheduleId,
                'times': newTime,
                'quantity' : quantity
            })
        })
        .then(response => response.status == '200'? response.json(): console.log(response))
        .then(_data => {
            console.log('success!');
            console.log(_data);
            fetchSlots();
        })
        .catch(e => console.log('failed - ' + e));
        
    }

    useEffect(() => {
        fetchSlots();
    }, [])

    const handleDelete = async(time) => {
        for (let i = 0; i < scheduleData.times.length; i++){
            if (scheduleData.times[i] == time){
                scheduleData.times.splice(i,1);
            }
        }
    }
    const handleCreate = async() =>{
        await createSchedule();
    }
    function convertToTime(num){
        if (num >= 10){
            return String(num);
        } else {
            return "0" + String(num);
        }
    }
    const renderTime = ({item}) => {
        return(
            <View
                style={{
                    marginTop: 12,
                    marginBottom: 12
                }}
            >
                <Tab title ={convertToTime(item.hour) + ":" + convertToTime(item.minutes)} text ={scheduleData.quantity + " pills"} onDelete = {() => handleDelete(item)} type='clock'/>

            </View>
        )
    }
    return(
        loading? (
            <View style={{
                flex:1,
                alignItems: 'center',
                justifyContent: 'center'
            }}>
                <ActivityIndicator size={'large'}/>

            </View>
        ) :
        <View
            style={{
                flex: 1,
                flexDirection: 'column',
                alignItems: 'center',
                justifyContent: 'space-between'
            }}
        >
            <Text style={[globalStyles.title, globalStyles.headerCenter]}>
                User's times
            </Text>
            <FlatList 
                data={scheduleData.times}
                renderItem={renderTime}
                // style={{
                //     minHeight: 100
                // }}
                // keyExtractor={item => }
            />
            <View
                style={{backgroundColor: "#474747", height: 1, width: "80%", marginBottom: 10}}
            ></View>
            
            <View style={{
                // flex: 1,
                flexDirection: 'row',
                // backgroundColor: '#474747'
                justifyContent: 'space-between',
                width: "100%",
                marginBottom: 10
            }}>
                <Text style={{lineHeight:40, marginLeft:"10%"}}>
                    NEW TIME
                </Text>
                <View style={{
                    flexDirection: 'row'
                }}>
                    <TextInput
                        keyboardType='numeric'
                        placeholder='hour'
                        onChangeText={value => setHours(value)}
                        style={{borderColor: "#474747", borderWidth: 1, textAlign:'center', width: 100, lineHeight: 40, height: 40}}

                    />
                    <Text style={{lineHeight: 40, fontWeight: 'bold'}}> : </Text>
                    <TextInput
                        keyboardType='numeric'
                        placeholder='minutes'
                        onChangeText={value => setMinutes(value)}
                        style={{borderColor: "#474747", borderWidth: 1, textAlign:'center', width: 100, lineHeight: 40, height: 40, marginRight:"10%"}}

                    />
                </View>
               
            </View>

            <View style={{
                width: "100%",
                flexDirection: 'row',
                justifyContent: 'space-between'
            }}>
                <Text style={{marginLeft:"10%"}}>
                    QUANTITY
                </Text>
                <TextInput
                    keyboardType='numeric'
                    placeholder={String(scheduleData.quantity)}
                    onChangeText={value => setQuanity(value)}
                    style={{borderColor: "#474747", borderWidth: 1, textAlign:'center', width: 100, lineHeight: 40, height: 40, marginRight:"10%"}}
                />
            </View>
           
            <View style={{marginVertical: 20}}>
                <PillButton 
                    text='Update Schedule'
                    onPress={()=>handleCreate()}
                />
            </View>
            
            {/* <PillButton 
                text='Test'
                onPress={()=>fetchSlots()}
            /> */}
        </View>
        
    )
}

export default OneSchedule;