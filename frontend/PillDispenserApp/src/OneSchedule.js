import React, { useEffect, useState } from 'react';
import { ActivityIndicator, FlatList, Text, TextInput, View } from 'react-native';
import PillButton from '../shared/pill-button';
import url from '../shared/variables'
import { getData } from '../shared/storage-utils';
import Tab from '../shared/tab';
function OneSchedule({navigation, route}){
    const [loading, setLoading] = useState(false);
    const [hours, setHours] = useState('');
    const [minutes, setMinutes] = useState('');
    const [quantity, setQuanity] = useState('');
    // const [slot, setSlot] = useState('');
    const [ids, setIds] = useState([]);
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
            // console.log(_data);
            const filteredData = findPillScheduleById(_data, scheduleId);
            setScheduleData(filteredData);
            // const arr = [_data[0]['id'], _data[1]['id']];
            // setIds(arr);
            // console.log(arr);
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
        const newTime = [...times, {'hour' : hours, 'minutes' : minutes}];
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
        })
        .catch(e => console.log('failed - ' + e));
        
    }

    useEffect(() => {
        fetchSlots();
        // console.log(times);
        // console.log('scheduleId: ' +scheduleId);
    }, [])


    const handleCreate = async() =>{
        // console.log(
        //     hours + ':' + minutes + ' quantity: ' + quantity
        // );
        const dataToSend = {
            'id': scheduleId,
                'times' : [
                    {
                        "hour": Number(hours),
                        "minutes": Number(minutes),
                    }
                ],
                'quantity' : quantity
        }
        console.log(dataToSend);
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
                <Tab title ={convertToTime(item.hour) + ":" + convertToTime(item.minutes)} type='clock'/>

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
                alignItems: 'center'
            }}
        >
            <Text>
                User's times
            </Text>
            <FlatList 
                data={scheduleData.times}
                renderItem={renderTime}
                // keyExtractor={item => }
            />
            <Text>
                NEW TIME
            </Text>
            <View style={{
                flexDirection: 'row'
            }}>
                <TextInput
                keyboardType='numeric'
                placeholder='hour'
                onChangeText={value => setHours(value)}
            />
            <Text>:</Text>
            <TextInput
                keyboardType='numeric'
                placeholder='minutes'
                onChangeText={value => setMinutes(value)}
            />
            </View>
            
            <TextInput
                keyboardType='numeric'
                placeholder='quantity'
                onChangeText={value => setQuanity(value)}
            />

            <PillButton 
                text='Add Time'
                onPress={()=>handleCreate()}
            />
            {/* <PillButton 
                text='Test'
                onPress={()=>fetchSlots()}
            /> */}
        </View>
        
    )
}

export default OneSchedule;