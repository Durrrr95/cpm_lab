% MIT License
% 
% Copyright (c) 2020 Lehrstuhl Informatik 11 - RWTH Aachen University
% 
% Permission is hereby granted, free of charge, to any person obtaining a copy
% of this software and associated documentation files (the "Software"), to deal
% in the Software without restriction, including without limitation the rights
% to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
% copies of the Software, and to permit persons to whom the Software is
% furnished to do so, subject to the following conditions:
% 
% The above copyright notice and this permission notice shall be included in all
% copies or substantial portions of the Software.
% 
% THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
% IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
% FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
% AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
% LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
% OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
% SOFTWARE.
% 
% This file is part of cpm_lab.
% 
% Author: i11 - Embedded Software, RWTH Aachen University

classdef Obstacle < handle
    
    properties
        id;
        role;
        type;
        
        % shape.name = {rectangle, circle,polygon}
        % R: shape.values = [length_R,width_R,orientation_R,centerX_R,centerY_R ];
        % C: shape.values = [radius_C,centerX_C,centerY_C ];
        % P: shape.values = [length_R,width_R,orientation_R,center_R ];
        shape; 
        
        
        % State.position = [x y]
        % State.orientation = [angle]
        % State.velocity = [speed]
        % State.time = [timestep]
        initialState;
        
        % trajectory{numStates} as cell array of states 
        trajectory;
        
    end
    methods
        
        function obj=Obstacle(New_Obstacle)
            numAttributes = size(New_Obstacle.Attributes,1);
            for k=1:numAttributes
                if strcmp(New_Obstacle.Attributes.Name, 'id')
                    obj.id = New_Obstacle.Attributes.Value;
                    continue;
                end
            end
            numChildren = size(New_Obstacle.Children,2);
            for k=1:numChildren
                switch New_Obstacle.Children(k).Name
                    case 'role'
                        obj.role = New_Obstacle.Children(k).Children.Data;
                    case 'type'
                        obj.type = New_Obstacle.Children(k).Children.Data;
                    case 'shape'
                        obj.shape.name = New_Obstacle.Children(k).Children.Name;
                        switch obj.shape.name
                            case 'rectangle'
                                length_R = NaN;
                                width_R = NaN;
                                % orientation and center may be omitted in XML if
                                % they equal zero
                                orientation_R = 0;
                                centerX_R = 0;
                                centerY_R = 0;
                                
                                numChild = size(New_Obstacle.Children(k).Children.Children,2);
                                for j=1:numChild
                                    switch New_Obstacle.Children(k).Children.Children(j).Name
                                        case 'length'
                                            length_R = str2double(New_Obstacle.Children(k).Children.Children(j).Children.Data);
                                        case 'width'
                                            width_R = str2double(New_Obstacle.Children(k).Children.Children(j).Children.Data);
                                        case 'orientation'
                                            orientation_R = str2double(New_Obstacle.Children(k).Children.Children(j).Children.Data);
                                            
                                        case 'center'
                                            
                                            for m=1:2
                                                switch New_Obstacle.Children(k).Children.Children(j).Children(m).Name
                                                    case 'x'
                                                        centerX_R = str2double(New_Obstacle.Children(k).Children.Children(j).Children(m).Children.Data);
                                                    case 'y'
                                                        centerY_R = str2double(New_Obstacle.Children(k).Children.Children(j).Children(m).Children.Data);
                                                end
                                            end
                                           
                                    end
                                end
                                obj.shape.values = [length_R,width_R,orientation_R,centerX_R,centerY_R ];
                            case 'circle'
                                radius_C = NaN;
                                centerX_C = 0;
                                centerY_C = 0;
                                
                                numChild = size(newObstacleNode.Children(k).Children.Children,2);
                                for j=1:numChild
                                    switch newObstacleNode.Children(k).Children.Children(j).Name
                                        case 'radius'
                                            radius_C = str2double(New_Obstacle.Children(k).Children.Children(j).Children.Data); 
                                        case 'center'
                                            for m=1:2
                                                switch New_Obstacle.Children(k).Children.Children(j).Children(m).Name
                                                    case 'x'
                                                        centerX_R = str2double(New_Obstacle.Children(k).Children.Children(j).Children(m).Children.Data);
                                                    case 'y'
                                                        centerY_R = str2double(New_Obstacle.Children(k).Children.Children(j).Children(m).Children.Data);
                                                end
                                            end
                                    end
                                end
                                obj.shape.values = [radius_C,centerX_C,centerY_C ];
                           
                            case 'polygon'
                                error('polygon shape not supported. please use minimum bounding rectangle');
                            otherwise
                                error(['unknown shape: ', obj.shape.name ]);
                        end
                    
                    case 'initialState'
                        obj.initialState = stateTostruct(New_Obstacle.Children(k));
                        
                    case 'trajectory'
                        numStates = size(New_Obstacle.Children(k).Children,2);
                        obj.trajectory = cell(numStates,1);
                        for j = 1:numStates
                            obj.trajectory{j,1} = stateTostruct(New_Obstacle.Children(k).Children(j));
                        end
                        
                end
            
            end
        end
      
        
    end
end