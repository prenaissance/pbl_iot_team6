import React, { useEffect, useState } from 'react';
import { Text, TextInput, View } from 'react-native';
import PillButton from '../shared/pill-button';
import url from '../shared/variables'
import { getData } from '../shared/storage-utils';
function NewSchedule({navigation, route}){
    const [loading, setLoading] = useState(false);
    const [hours, setHours] = useState('');
    const [minutes, setMinutes] = useState('');
    const [quantity, setQuanity] = useState('');
    const [slot, setSlot] = useState('');
    const [ids, setIds] = useState([]);
    const { profileId } = route.params;
    const fetchSlots = async() => {
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
            setIds(arr);
            console.log(arr);
        })
    
        .catch(e => {
            console.log('failed - ' + e.data);
        });
    }

    const createSchedule = async() => {
        const token = await getData('token');
        console.log('awaiting...' + token);
        // const slotId;
        fetch(url + 'api/pills/slots/'+ids[Number(slot)-1]+'/schedules', {
            method: 'POST',
            headers:{
                Accept: 'application/json',
                'Content-Type' : 'application/json',
                'Authorization' : 'Bearer '+token,
            },
            body:{
                'pillSchedules':[{
                    'times' : [
                        {
                            "hour": hours,
                            "minutes": minutes,
                        }
                    ],
                    'profileId' : profileId,
                    'quantity' : quantity

                    
                }]
                
            }
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
    }, [])


    const handleCreate = async() =>{
        console.log(
            hours + ':' + minutes + ' quantity: ' + quantity
        );
        const dataToSend = {
            'id': ids[Number(slot)-1],
                'times' : [
                    {
                        "hour": Number(hours),
                        "minutes": Number(minutes),
                    }
                ]
        }
        console.log(dataToSend);
        await createSchedule();
        
    }
    return(
        <View>
            <Text>
                New Schedule
            </Text>
            <TextInput
                keyboardType='numeric'
                placeholder='hour'
                onChangeText={value => setHours(value)}
            />
            <TextInput
                keyboardType='numeric'
                placeholder='minutes'
                onChangeText={value => setMinutes(value)}
            />
            <TextInput
                keyboardType='numeric'
                placeholder='quantity'
                onChangeText={value => setQuanity(value)}
            />
            <TextInput
                keyboardType='numeric'
                placeholder='slot'
                onChangeText={value => setSlot(value)}
            />

            <PillButton 
                text='Create'
                onPress={()=>handleCreate()}
            />
            {/* <PillButton 
                text='Test'
                onPress={()=>fetchSlots()}
            /> */}
        </View>
        
    )
}

export default NewSchedule;