import React from 'react';
import {TextInput} from 'react-native';

function SearchBar(){
    return(
        <TextInput
            placeholder='search'
            style={{
                backgroundColor: '#e9e9e9',
                width: 400,
                height: 40,
                borderRadius: 50,
            }}
        />
    )
}

export default SearchBar;