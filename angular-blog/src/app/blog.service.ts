import { Injectable } from '@angular/core';
import { MdPost, Response } from './mdpost';

@Injectable({
    providedIn: 'root'
})
export class BlogService {

    constructor() { }

    fetchPosts(directory: string): Promise<string[]> {
        return fetch(`/api/${directory}`).then((res) => {
            if (res.ok)
                return Promise.resolve(res.json());
            else
                return Promise.reject(new Error(String(res.status)));
                
        });
    }

    getPost(directory: string, postid: number): Promise<string> {
        return fetch(`/api/${directory}/${postid}`).then((res) => {
            if (res.ok) {
                return Promise.resolve(res.text());
            }
            else
                return Promise.reject(new Error(String(res.status)));
        });
    }

    createPost(directory: string): Promise<Response> {
        return fetch(`/api/${directory}/`, 
            {
                'method': 'POST',
                'headers': {
                    'Content-Type': 'text/plain'
                },
                'body': ""
            }
        ).then(res => {
            if (res.ok)
                return Promise.resolve(res.json());
            else
                return Promise.reject(new Error(String(res.status)));
        });
    }

    setPost(directory: string, post: MdPost): Promise<string> {
        console.log(post.content);
        return fetch(`/api/${directory}/${post.postid}`, 
            {
                'method': 'PUT',
                'headers': {
                    'Content-Type': 'text/plain'
                },
                'body': `${post.content}`
            }
        ).then(res => {
            if (res.ok)
                return Promise.resolve(res.text());
            else
                return Promise.reject(new Error(String(res.status)));
        });
    }

    deletePost(directory: string, postid: number): Promise<void> {
        return fetch(`/api/${directory}/${postid}`, {'method': 'DELETE'})
        .then((res) => {
            if (res.ok)
                return Promise.resolve();
            else
                return Promise.reject(new Error(String(res.status)));
        });
    }

}
