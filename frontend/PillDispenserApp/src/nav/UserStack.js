import React from 'react';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import NewUser from '../NewUser';
import Users from '../Users';
import OneUser from '../OneUser';
import OneSchedule from '../OneSchedule';

const Stack = createNativeStackNavigator();

function UserStack(){

    return(
        <Stack.Navigator initialRouteName='Users'
            screenOptions={{headerShown: false}}
        >
            <Stack.Screen name="Users" component={Users} />
            <Stack.Screen name="NewUser" component={NewUser} />
            <Stack.Screen name="OneUser" component={OneUser}/>
            <Stack.Screen name="OneSchedule" component={OneSchedule}/>
        </Stack.Navigator>
    )
}

export default UserStack;