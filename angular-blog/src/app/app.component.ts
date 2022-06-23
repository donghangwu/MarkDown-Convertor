import { Component, HostListener } from '@angular/core';
import { MdPost } from './mdpost';
import { BlogService } from './blog.service';

enum AppState { List = 0, Edit = 1, Preview = 2 };

@Component({
    selector: 'app-root',
    templateUrl: './app.component.html',
    styleUrls: ['./app.component.css']
})
export class AppComponent {

    posts: string[] = [];
    currentPost: MdPost = new MdPost();
    appState: AppState = AppState.List;
    directory: string = "";

    constructor(private blogService: BlogService) {
        if (window.location.hash == '') {
            window.location.hash = '/';
        }
        this.onHashChange();
    }

    fetchPosts(dir: string) {
        this.blogService.fetchPosts(dir)
        .then(res => this.posts = res)
        .catch(err => {
            this.posts = [];
            console.error(err);
        });
    }

    get(dir: string, postid: number) {
        if (postid != 0) {
            this.blogService.getPost(dir, postid)
            .then((res) => {
                this.currentPost = new MdPost();
                this.currentPost.content = res;
                this.currentPost.directory = dir;
                this.currentPost.postid = postid;
            })
            .catch(err => {
                this.currentPost = new MdPost();
                window.location.hash = '/';
                console.error(err);
            });

        }
        else
            this.currentPost = new MdPost();
    }

    create(dir: string) {
        if (dir != "") {
            this.blogService.createPost(dir).then((res) => {})
            .catch((err) => {
                console.error(err);
                window.location.hash = '/';
            });
        }
        else {
            console.error("Directory name cannot be empty!");
        }
    }
    
    @HostListener('window:hashchange')
    onHashChange(): void {
        let reList = /^#\/list\/[A-Za-z]+\/?$/;
        let reEdit = /^#\/edit\/[A-Za-z]+\/\d+\/?$/;
        let rePreview = /^#\/preview\/[A-Za-z]+\/\d+\/?$/;

        let pathList: string[] = window.location.hash.split('/');
        
        if (reList.test(window.location.hash)) {
            // console.log('list');

            this.appState = AppState.List;
            this.directory = pathList[2];
            // let dir: string = pathList[1];
            this.fetchPosts(this.directory);
        }
        else if (reEdit.test(window.location.hash)) {
            // console.log('edit');

            this.appState = AppState.Edit;
            this.directory = pathList[2];
            // let dir: string = pathList[2];
            let postid: number = parseInt(pathList[3]);
            this.get(this.directory, postid);
        }
        else if (rePreview.test(window.location.hash)) {
            // console.log('preview');

            this.appState = AppState.Preview;
            this.directory = pathList[2];
            // let dir: string = pathList[2];
            let postid: number = parseInt(pathList[3]);
            this.get(this.directory, postid);
        }
        else {
            // console.log('else');

            this.directory = "";
            window.location.hash = '/';
        }
    }

    // event handlers for list component events
    openPost(post: string) {
        window.location.hash = `/edit/${this.directory}/${post}`;
    }
    listPost(dir: string) {
        // console.log(dir);
        this.fetchPosts(dir);
        window.location.hash = `/list/${dir}`;
    }
    newPost(dir: string) {
        this.create(dir);
        this.fetchPosts(dir);
        this.directory = dir;
        window.location.hash = `/list/${dir}`;
    }
    // event handlers for edit component events
    previewPost(post: MdPost) {
        window.location.hash = `/preview/${post.directory}/${post.postid}`;
    }
    savePost(post: MdPost) {
        console.log(post);
        if (post.postid > 0) {
            this.blogService.setPost(post.directory, post).then(res => {}).catch(err => console.error(err));
            this.fetchPosts(post.directory);
        }
        else {
            console.error('Wrong postid!');
        }
    }
    deletePost(post: MdPost) {
        this.blogService.deletePost(post.directory, post.postid)
        .then(res => {
            this.currentPost = new MdPost();
            window.location.hash = `/list/${post.directory}`;
        })
        .catch(err => console.error(err));
        this.fetchPosts(post.directory);

    }
    // event handlers for preview component events
    editPost(post: MdPost) {
        window.location.hash = `/edit/${post.directory}/${post.postid}`;
    }

}
