### Description

This example shows how to add normals to vertices. This is identical to adding normals to any type of cells. The only difference from [Normals to/from a Polydata]]([Add/Get) is

<source lang="cpp">
polydata->GetPointData()->SetNormals(PointNormalsArray.Get());
</source>

<source lang="cpp">
vtkSmartPointer<vtkDoubleArray> PointNormalsRetrieved = vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetNormals());
</source>

are changed to

<source lang="cpp">
polydata->GetCellData()->SetNormals(CellNormalsArray.Get());
</source>

and

<source lang="cpp">
vtkSmartPointer<vtkDoubleArray> CellNormalsRetrieved = vtkDoubleArray::SafeDownCast(polydata->GetCellData()->GetNormals());
</source>
