import React from 'react';
import {TouchableOpacity, Text} from 'react-native';
function PillButton(props){

    return(
        <TouchableOpacity
        style={{
            width:300,
            height: 73, 
            borderRadius: 10,
            backgroundColor: '#00C780',
            color: 'white',
            justifyContent: 'center',
            alignContent: 'center',
            // marginTop: 265
        }}
        onPress={() => props.onPress?.()}
    >
        <Text style={{
            textAlign: 'center',
            color: 'white',
            fontSize: 25
        }}>{props.text}</Text>
    </TouchableOpacity>
    )
}

export default PillButton;