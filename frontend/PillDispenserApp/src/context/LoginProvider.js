import { useFocusEffect } from '@react-navigation/native';
import React, {createContext, useContext, useState} from 'react'
import { getData } from '../../shared/storage-utils';

const LoginContext = createContext();
export const useLogin = () => useContext(LoginContext);
const LoginProvider = ({children}) => {
    const [isLoggedIn, setIsLoggedIn] = useState(false);
    return(
        <LoginContext.Provider value={{isLoggedIn, setIsLoggedIn}}>
            {children}
        </LoginContext.Provider>
    )
}

export default LoginProvider;