import React from 'react';
import { Text, TouchableOpacity, View } from 'react-native';
import Tab from '../shared/tab';

function Containers({navigation}){
    return(
        <View
            style={{
                flex: 1,
            }}
        >
            <Text>
                CONTAINERS
            </Text>
                    <View 
                        style={{
                            marginTop:12
                        }}
                    >
                        <Tab
                                title='Container A'
                                text='Tap to see details'
                                image='user'
                                onPress={() => navigation.navigate("OneContainer")}
                            />
                    </View>
                        
                    <View
                        style={{
                            marginTop:12
                        }}
                    >
                        <Tab
                            title='Container B'
                            text='Tap to see details'
                            image='user'
                            onPress={() => navigation.navigate("OneContainer")}

                        />
                    </View>
                        

              
        </View>
    )
}

export default Containers;