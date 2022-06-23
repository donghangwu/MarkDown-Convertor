import { Component, Input, Output, OnInit, EventEmitter, OnChanges } from '@angular/core';
import { Parser, HtmlRenderer } from 'commonmark';
import { MdPost } from '../mdpost';

function markdownToHtml(markdownString: string): string {
    let reader = new Parser();
    let writer = new HtmlRenderer();
    let parsed = reader.parse(markdownString);
    return writer.render(parsed);
}

@Component({
    selector: 'app-preview',
    templateUrl: './preview.component.html',
    styleUrls: ['./preview.component.css']
})
export class PreviewComponent implements OnInit, OnChanges {

    @Input() post: MdPost;
    @Output() editPost = new EventEmitter<MdPost>();

    // title: string;
    body: string;

    
    constructor() { }
    
    ngOnInit(): void {
        // this.title = markdownToHtml(this.post.title);
        this.body = markdownToHtml(this.post.content);
    }

    ngOnChanges(): void {
        // this.title = markdownToHtml(this.post.title);
        this.body = markdownToHtml(this.post.content);
    }

}
