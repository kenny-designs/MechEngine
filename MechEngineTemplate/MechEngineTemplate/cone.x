xof 0303txt 0032

Frame Root {
  FrameTransformMatrix {
     1.000000, 0.000000, 0.000000, 0.000000,
     0.000000,-0.000000, 1.000000, 0.000000,
     0.000000, 1.000000, 0.000000, 0.000000,
     0.000000, 0.000000, 0.000000, 1.000000;;
  }
  Frame Cone {
    FrameTransformMatrix {
       1.000000, 0.000000, 0.000000, 0.000000,
       0.000000, 1.000000, 0.000000, 0.000000,
       0.000000, 0.000000, 1.000000, 0.000000,
       0.000000, 0.000000, 0.000000, 1.000000;;
    }
    Mesh { // Cone mesh
      10;
       0.000000; 1.000000;-1.000000;,
       0.707107; 0.707107;-1.000000;,
       1.000000;-0.000000;-1.000000;,
       0.707107;-0.707107;-1.000000;,
      -0.000000;-1.000000;-1.000000;,
       0.000000; 0.000000; 1.000000;,
      -0.707107;-0.707107;-1.000000;,
      -1.000000; 0.000000;-1.000000;,
      -0.707107; 0.707107;-1.000000;,
      -0.000000; 0.000000;-1.000000;;
      12;
      3;1,5,0;,
      3;2,5,1;,
      3;3,5,2;,
      3;4,5,3;,
      3;6,5,4;,
      3;7,5,6;,
      3;8,5,7;,
      3;0,5,8;,
      4;2,9,4,3;,
      4;9,2,1,0;,
      4;9,0,8,7;,
      4;9,7,6,4;;
      MeshNormals { // Cone normals
        12;
         0.347408; 0.838717; 0.419359;,
         0.838717; 0.347408; 0.419359;,
         0.838717;-0.347408; 0.419358;,
         0.347408;-0.838717; 0.419359;,
        -0.347408;-0.838717; 0.419358;,
        -0.838717;-0.347408; 0.419358;,
        -0.838717; 0.347408; 0.419358;,
        -0.347408; 0.838717; 0.419358;,
         0.000000;-0.000000;-1.000000;,
         0.000000; 0.000000;-1.000000;,
        -0.000000; 0.000000;-1.000000;,
         0.000000;-0.000000;-1.000000;;
        12;
        3;0,0,0;,
        3;1,1,1;,
        3;2,2,2;,
        3;3,3,3;,
        3;4,4,4;,
        3;5,5,5;,
        3;6,6,6;,
        3;7,7,7;,
        4;8,8,8,8;,
        4;9,9,9,9;,
        4;10,10,10,10;,
        4;11,11,11,11;;
      } // End of Cone normals
      MeshMaterialList { // Cone material list
        1;
        12;
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0;
        Material Material_001 {
           0.818293; 0.793692; 0.793692; 1.000000;;
           96.078431;
           0.500000; 0.500000; 0.500000;;
           0.000000; 0.000000; 0.000000;;
          TextureFilename {"BlueCone.bmp";}
        }
      } // End of Cone material list
    } // End of Cone mesh
  } // End of Cone
} // End of Root