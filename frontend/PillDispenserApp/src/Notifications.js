import React, {useEffect, useState} from "react";
import { ActivityIndicator, Alert, FlatList, Text, View } from "react-native";
import { globalStyles } from "../shared/style-sheet";
import {url} from "../shared/variables";
import { getData } from "../shared/storage-utils";
import Tab from "../shared/tab";
function Notifications({navigation}){
    const [data, setData] = useState([]);
    const [loading, setLoading] = useState(false);
    const fetchNotifs = async () => {
        setLoading(true);
        try {
            const token = await getData('token');
            console.log('awaiting... ' + token);
    
            const response = await fetch(url + 'api/events', {
                method: 'GET',
                headers: {
                    Accept: 'application/json',
                    // 'Content-Type' : 'application/json', // Not needed for GET requests
                    'Authorization': 'Bearer ' + token,
                },
            });
    
            if (response.ok) {
                const data = await response.json();
                console.log(data);
                setData(data);
            } else {
                console.error('Error:', response.status, response.statusText);
                Alert.alert('Error', `Request failed with status ${response.status}`);
            }
        } catch (e) {
            Alert.alert('Error', e.message);
        } finally {
            setLoading(false);
        }
    };
    useEffect(() => {
        fetchNotifs();
    }, [])

    const renderItem = ({item}) => {
        return(
            <Tab text={item.createdAt}/>
        )
    }
    return(
        <View>
            <Text style={[globalStyles.title, globalStyles.header]}>
                NOTIFICATIONS
            </Text>
            {loading? <ActivityIndicator size={"large"}/>
            : 
            <FlatList renderItem={renderItem} data={data}/>

            }
        </View>
    )
}

export default Notifications;