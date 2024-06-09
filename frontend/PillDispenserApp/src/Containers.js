import React, { useEffect, useState } from 'react';
import { ActivityIndicator, FlatList, Text, TouchableOpacity, View } from 'react-native';
import Tab from '../shared/tab';
import { getData } from '../shared/storage-utils';
function Containers({navigation}){
    const [loading, setLoading] = useState(false)
    const [data, setData] = useState([])
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
            console.log(_data);
            // const arr = [_data[0]['id'], _data[1]['id']];
            // setIds(arr);
            // console.log(arr);
            setData(_data);
            setLoading(false);
        })
    
        .catch(e => {
            console.log('failed - ' + e);
        });
    }

    useEffect(() => {
        fetchSlots();
    }, [navigation])

    const renderItem = ({item}) => {
        return(
            <View style={{marginTop: 12}}>
                <Tab 
                    title={item.pillName}
                    text='Tap to see details'
                />
            </View>
        )
    }
    return(
        <View
        
            style={{
                flex: 1,
                // alignItems: 'center'
            }}
        >
            <Text>
                CONTAINERS
            </Text>
            {loading 
                ?(
                    <ActivityIndicator size={'large'}/>
                ): (
                    <View
                style={{
                    // flex: 1,
                    alignItems: 'center'
                }}
            >
                    <FlatList
                        data={data}
                        renderItem={renderItem}
                        keyExtractor={item => item.id}
                    />
                    </View>
                )
            }
            </View>
                     
        
    )
}

export default Containers;