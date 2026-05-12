# File management and File panel
File management creates the file trees. And the FilePanel request for loading the children when the user clicked the folder button.

## File panel 
FilePanel listen for events in file management. When a new workspace is crreated. FilePanel creates a corresponding WorkspaceBox. Workspaces are required for certain language servers, including rust-analyzer. Workspace is not yet implemented in language server client.

*file panel*
- *workspace box 1*
  - *root folder*
    - *folders*
    - *files*
- *workspace box 2*
  - *root folder*
    - *folders*
    - *files*
