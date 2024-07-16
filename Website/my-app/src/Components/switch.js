import "./switch.css"
import { useState } from 'react'

export const Switch = ({label, toggled, onClick }) => {
    const [State, setState] = useState(toggled);

    const callback = () => {
        setState(!State)
        onClick(!State)
    }

    return (
        <label>
            <input type="checkbox" defaultChecked={State} onClick={callback} />
            <span />
            {/* <strong>{label}</strong> */}
        </label>
    );
};