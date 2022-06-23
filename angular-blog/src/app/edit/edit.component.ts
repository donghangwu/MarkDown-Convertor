import { Component, Input, OnInit, Output, EventEmitter, OnChanges } from '@angular/core';
import { MdPost } from '../mdpost';

@Component({
    selector: 'app-edit',
    templateUrl: './edit.component.html',
    styleUrls: ['./edit.component.css']
})
export class EditComponent implements OnInit, OnChanges {

    @Input() post: MdPost = new MdPost();
    @Output() savePost = new EventEmitter<MdPost>();
    @Output() deletePost = new EventEmitter<MdPost>();
    @Output() previewPost = new EventEmitter<MdPost>();
    
    constructor() { }

    ngOnInit(): void {
    }

    ngOnChanges() {}

}
