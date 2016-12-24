
/* slow_activex.p - */

{slib/slibos.i}

define var chExcelApplication   as com-handle no-undo.
define var chWorkbook           as com-handle no-undo.
define var chWorksheet          as com-handle no-undo.
define var chPivot              as com-handle no-undo.
define var chWorksheetRange     as com-handle no-undo.
define var chChart              as com-handle no-undo.
define var iRowNum              as int no-undo.



etime( yes ).

create "Excel.Application" chExcelApplication.
chExcelApplication:visible = False.

chWorkbook  = chExcelApplication:Workbooks:add( ).
chWorkSheet = chExcelApplication:Sheets:Item(1).
chWorkSheet:DisplayRightToLeft = False.
chWorkSheet:name = "Data".

chWorkSheet:Columns( "A" ):ColumnWidth = 5.86.
chWorkSheet:Columns( "B" ):ColumnWidth = 4.71.
chWorkSheet:Columns( "C" ):ColumnWidth = 12.43.
chWorkSheet:Columns( "D" ):ColumnWidth = 14.29.
chWorkSheet:Columns( "E" ):ColumnWidth = 17.14.
chWorkSheet:Columns( "F" ):ColumnWidth = 28.71.
chWorkSheet:Columns( "G" ):ColumnWidth = 13.57.
chWorkSheet:Columns( "H" ):ColumnWidth = 13.29.



iRowNum = 1.

chWorkSheet:Range( "A1:H1" ):Select.
chExcelApplication:Selection:Font:ColorIndex        = 2. /* white */
chExcelApplication:Selection:Font:FontStyle         = "Bold Italic".
chExcelApplication:Selection:Interior:Pattern       = 1. /* xlSolid */
chExcelApplication:Selection:Interior:ColorIndex    = 11. /* white */

chExcelApplication:Selection:Borders( 5 /* xlDiagonalDown */        ):LineStyle = -4142.    /* xlNone */
chExcelApplication:Selection:Borders( 6 /* xlDiagonalUp */          ):LineStyle = -4142.    /* xlNone */
chExcelApplication:Selection:Borders( 7 /* xlEdgeLeft */            ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 7 /* xlEdgeLeft */            ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 8 /* xlEdgeTop */             ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 8 /* xlEdgeTop */             ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 9 /* xlEdgeBottom */          ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 9 /* xlEdgeBottom */          ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 10 /* xlEdgeRight */          ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 10 /* xlEdgeRight */          ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 11 /* xlInsideHorizontal */   ):LineStyle = -4142.    /* xlNone */
chExcelApplication:Selection:Borders( 12 /* xlInsideVertical */     ):LineStyle = -4142.    /* xlNone */

chWorkSheet:Range( "A1" ):Value = "Order".
chWorkSheet:Range( "B1" ):Value = "Line".
chWorkSheet:Range( "C1" ):Value = "Order Date".
chWorkSheet:Range( "D1" ):Value = "Order Month".
chWorkSheet:Range( "E1" ):Value = "Part".
chWorkSheet:Range( "F1" ):Value = "Customer".
chWorkSheet:Range( "G1" ):Value = "Country".
chWorkSheet:Range( "H1" ):Value = "Extended Price".

chWorkSheet:Range( "A2" ):Select.
chExcelApplication:ActiveWindow:FreezePanes = True.



for each Customer
    where customer.custnum = 1
    no-lock,
    
    each Order of Customer
    no-lock,
    
    each OrderLine of Order
    no-lock,
    
    first Item of OrderLine
    no-lock:

    iRowNum = iRowNum + 1.

    chWorkSheet:Range( "A" + string( iRowNum ) ):Value          = Order.OrderNum.
    chWorkSheet:Range( "B" + string( iRowNum ) ):Value          = OrderLine.LineNum.
    chWorkSheet:Range( "C" + string( iRowNum ) ):Value          = Order.OrderDate.
    chWorkSheet:Range( "D" + string( iRowNum ) ):Formula        = '=TEXT(C' + string( iRowNum ) + ',"yyyy-mm")'.
    chWorkSheet:Range( "E" + string( iRowNum ) ):Value          = Item.ItemName.
    chWorkSheet:Range( "F" + string( iRowNum ) ):Value          = Customer.Name.
    chWorkSheet:Range( "G" + string( iRowNum ) ):Value          = Customer.Country.
    chWorkSheet:Range( "H" + string( iRowNum ) ):Value          = OrderLine.ExtendedPrice.
    
    chWorkSheet:Range( "A" + string( iRowNum ) + ":H" + string( iRowNum ) ):Select.
    chExcelApplication:Selection:Interior:Pattern       = 1. /* xlSolid */
    chExcelApplication:Selection:Interior:ColorIndex    = ( if iRowNum mod 2 = 0 then 15 /* gray */ else 2 /* white */ ).

end. /* for each */

chWorkSheet:Range( "A2:H" + string( iRowNum ) ):Select.
chExcelApplication:Selection:Borders( 5 /* xlDiagonalDown */        ):LineStyle = -4142.    /* xlNone */
chExcelApplication:Selection:Borders( 6 /* xlDiagonalUp */          ):LineStyle = -4142.    /* xlNone */
chExcelApplication:Selection:Borders( 7 /* xlEdgeLeft */            ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 7 /* xlEdgeLeft */            ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 8 /* xlEdgeTop */             ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 8 /* xlEdgeTop */             ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 9 /* xlEdgeBottom */          ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 9 /* xlEdgeBottom */          ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 10 /* xlEdgeRight */          ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 10 /* xlEdgeRight */          ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 11 /* xlInsideHorizontal */   ):LineStyle = -4142.    /* xlNone */
chExcelApplication:Selection:Borders( 12 /* xlInsideVertical */     ):LineStyle = -4142.    /* xlNone */

chWorkSheet:Range( "A2:A" + string( iRowNum ) ):NumberFormat   = "###".
chWorkSheet:Range( "B2:B" + string( iRowNum ) ):NumberFormat   = "###".
chWorkSheet:Range( "C2:C" + string( iRowNum ) ):NumberFormat   = "dd/mm/yyyy".
chWorkSheet:Range( "H2:H" + string( iRowNum ) ):NumberFormat   = "#,##0.00".



iRowNum = iRowNum + 1.

chWorkSheet:Range( "H" + string( iRowNum ) ):Formula = '=SUM(H2:H' + string( iRowNum - 1 ) + ')'.
chWorkSheet:Range( "H" + string( iRowNum ) ):NumberFormat = "#,##0.00".

chWorkSheet:Range( "A" + string( iRowNum ) + ":H" + string( iRowNum ) ):Select.
chExcelApplication:Selection:Borders( 5 /* xlDiagonalDown */        ):LineStyle = -4142.    /* xlNone */
chExcelApplication:Selection:Borders( 6 /* xlDiagonalUp */          ):LineStyle = -4142.    /* xlNone */
chExcelApplication:Selection:Borders( 7 /* xlEdgeLeft */            ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 7 /* xlEdgeLeft */            ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 8 /* xlEdgeTop */             ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 8 /* xlEdgeTop */             ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 9 /* xlEdgeBottom */          ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 9 /* xlEdgeBottom */          ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 10 /* xlEdgeRight */          ):LineStyle = 1.        /* xlContinuous */
chExcelApplication:Selection:Borders( 10 /* xlEdgeRight */          ):Weight    = -4138.    /* xlMedium */
chExcelApplication:Selection:Borders( 11 /* xlInsideHorizontal */   ):LineStyle = -4142.    /* xlNone */
chExcelApplication:Selection:Borders( 12 /* xlInsideVertical */     ):LineStyle = -4142.    /* xlNone */

chExcelApplication:Selection:Interior:Pattern       = 1. /* xlSolid */
chExcelApplication:Selection:Interior:ColorIndex    = 2. /* white */



chWorkbook:SaveAs( os_getNextFile( session:temp-dir + "oleauto_new.xls" ), -4143 /* xlNormal */, "", "", False, False, 1 /* xlNoChange */ ).

chWorkSheet:Range( "A2" ):Select.
chExcelApplication:Visible = true.

release object chExcelApplication.      
release object chWorkbook.
release object chWorksheet.

message etime( no ) view-as alert-box.
