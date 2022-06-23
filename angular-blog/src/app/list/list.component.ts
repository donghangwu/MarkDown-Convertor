import { Component, OnInit, Input, Output, EventEmitter, OnChanges } from '@angular/core';

@Component({
    selector: 'app-list',
    templateUrl: './list.component.html',
    styleUrls: ['./list.component.css']
})
export class ListComponent implements OnInit, OnChanges {

    @Input() posts: string[];
    @Output() openPost = new EventEmitter<string>();
    @Output() newPost = new EventEmitter<string>();
    @Output() listPost = new EventEmitter<string>();

    postDir: string = "";


    constructor() { }

    ngOnInit(): void {
    }

    ngOnChanges() {
    }

}
