#ifndef CCOORDSYSTEM_H
#define CCOORDSYSTEM_H

#include "d3dx9.h"

// ������������ �������.
class CCoordSystem : public CMain
{
    CCoordSystem *_parent; // ������������ ������������ �������
    CCoordSystem *_next;   // ��������� ������������ ������� � ����� �� ���������
    CCoordSystem *_child;  // ������� �������� ������������ ������

    D3DXVECTOR3     _pos;   // ����� � ������������ ������������ �������, ������ ������� ��������� ������ ������������ �������
    D3DXQUATERNION  _rot;   // ���������� �������� ������ ������������ ������� ������������ ������������ ������������ �������

    D3DXMATRIX      _matrix;  // ��������� �������
    
    // for internal use
    D3DXQUATERNION  _cur_rot; // ������� �������. ������� �� �������� ������������ ������������ ������


    void update_chain(CCoordSystem *parent)
    {
        _parent = parent;
        if (parent->_child)
        {
            // � ������������ ������� ������� ��������
            _next = parent->_child;
            parent->_child = this;

        } else
        {
            // � ������������ ������� ����������� ��������
            _next = 0;
            parent->_child = this;

        }
        _child = 0;
    }

    void update_matrix(void)
    {
        D3DXMATRIX mat;

        if (_parent)
        {
            D3DXVECTOR3 ppos;
            ppos = _parent->transform(_pos);
            D3DXQuaternionMultiply(&_cur_rot, &_parent->_cur_rot ,&_rot);
            D3DXMatrixAffineTransformation(&_matrix, 1.0, 0, &_cur_rot, &ppos);

        } else
        {
            D3DXMatrixAffineTransformation(&_matrix, 1.0, 0, &_rot, &_pos);
            _cur_rot = _rot;
        }

        if (_child) _child->update_matrix_parent_request();
    }

    void update_matrix_parent_request(void)
    {
        update_matrix();
        if (_next) _next->update_matrix_parent_request();
    }

public:

    // root coord system with default orientation
    CCoordSystem(void)
    {
        _parent = 0;
        _next = 0;
        _child = 0;

        _pos = D3DXVECTOR3(0,0,0);
        _rot = D3DXQUATERNION(0,0,0,1);
        update_matrix();
    }
    // root coord system with specified orientation
    CCoordSystem(const D3DXVECTOR3& pos, const D3DXVECTOR3&axis, const float angle)
    {
        _parent = 0;
        _next = 0;
        _child = 0;
        _pos = pos;

        set_rotation(axis,angle);
    }

    // child coord system with default orientation
    CCoordSystem(CCoordSystem *parent)
    {
        update_chain(parent);
        _pos = D3DXVECTOR3(0,0,0);
        _rot = D3DXQUATERNION(0,0,0,1);
        update_matrix(); // its update matrix and current quaternion
    }


    // child coord system with specified orientation
    CCoordSystem(CCoordSystem *parent, const D3DXVECTOR3& pos, const D3DXVECTOR3&axis, const float angle)
    {
        update_chain(parent);
        _pos = pos;
        set_rotation(axis,angle);
    }

    // set rotation for coord system
    void set_rotation(const D3DXVECTOR3&axis, const float angle)
    {
        double s = sin(angle*0.5);
        double c = cos(angle*0.5);

        _rot.x = (float) (s * axis.x);
        _rot.y = (float) (s * axis.y);
        _rot.z = (float) (s * axis.z);
        _rot.w = (float) c;

        update_matrix(); // its update matrix and current quaternion
    }


    // transform vertext position to absolute coordinates
    D3DXVECTOR3 transform(D3DXVECTOR3 &v)
    {
        D3DXVECTOR3 out;
        D3DXVec3TransformCoord(&out, &v, &_matrix);
        return out;
    }


};


#endif