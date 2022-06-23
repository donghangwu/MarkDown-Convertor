export class MdPost {
    directory: string = "";
    postid: number = 0;
    content: string = "";
};

export class Response {
    id: number = 0;
};

export const sampleMdPosts = [
    {
        "directory": "Shoes",
        "postid": 1,
        "content": "#Title1"
    },
    {
        "directory": "Shoes",
        "postid": 2,
        "content": "#Title2"
    },
    {
        "directory": "Cleats",
        "postid": 3,
        "content": "#Title3"
    }
];