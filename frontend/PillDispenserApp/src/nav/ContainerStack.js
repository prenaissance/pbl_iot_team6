import React from 'react';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import Containers from '../Containers';
import OneContainer from '../OneContainer';

const Stack = createNativeStackNavigator();

function ContainerStack(){

    return(
        <Stack.Navigator initialRouteName='Users'
            screenOptions={{headerShown: false}}
        >
            <Stack.Screen name="Containers" component={Containers} />
            <Stack.Screen name="OneContainer" component={OneContainer} />
        </Stack.Navigator>
    )
}

export default ContainerStack;