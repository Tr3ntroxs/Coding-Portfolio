"use client";

import { useState } from "react";

async function makePostRequest() {
    const response = await fetch(`${process.env.NEXT_URL}/api/hello`, {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify({name: "John Doe"}),
    });

    const data = await response.json();
    return {data};
}

export default async function Friends(){ 
    const [message, setMessage] = useState("");
    const onClick = async () => {
        const {data} = await makePostRequest();
        setMessage(data.message);
    };
    return (
        <div>
            <h1>Friends Page: {message} <button onClick={onClick}>Click Me</button></h1>
        </div>
    );
}