import { notFound } from 'next/navigation';

async function fetchUser(userId: string) {
    const res = await fetch(`https://jsonplaceholder.typicode.com/users/${userId}`);
    if (!res.ok) { 
        return null;
    }
    return res.json();
}

export default async function UserPage({
    params}: {
        params: Promise<{ userId: string }>;
    }) {

    const { userId } = await params;
    const user = await fetchUser(userId);

    if (!user) {
        notFound();
    }

    return (
        <div>
            <h1>{user.name}</h1>
        </div>
    )
}