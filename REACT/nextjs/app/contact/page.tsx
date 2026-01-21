import { Metadata } from "next";
import ButtonComponent from "./button";

export const metadata: Metadata = {
  title: "About Us | Website",
  description: "Learn more about our website and team",
  keywords: ["about", "website", "team"],
  twitter: {
    card: "summary_large_image",
    title: "About Us | Website",
  },
};

export default async function Contact() {
    console.log("S or C?")

    const response = await fetch("https://jsonplaceholder.typicode.com/users");
    console.log(response);
    const posts = await response.json()
    console.log(posts);

    return (
    <div>
        <ButtonComponent />
    </div>
    );
}