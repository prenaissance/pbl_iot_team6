import React from 'react';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import Containers from '../Containers';
import OneContainer from '../OneContainer';
import DeviceConfig from '../DeviceConfig';

const Stack = createNativeStackNavigator();

function ContainerStack(){

    return(
        <Stack.Navigator initialRouteName='Containers'
            screenOptions={{headerShown: false}}
        >
            <Stack.Screen name="Containers" component={Containers} />
            <Stack.Screen name="OneContainer" component={OneContainer} />
            <Stack.Screen name="DeviceConfig" component={DeviceConfig} />

        </Stack.Navigator>
    )
}

export default ContainerStack;