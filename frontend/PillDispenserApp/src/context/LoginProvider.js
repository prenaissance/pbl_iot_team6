import React, {createContext, useContext, useState} from 'react'

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