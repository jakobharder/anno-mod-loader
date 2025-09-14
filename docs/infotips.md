# InfoTips

You can patch `data/infotips/export.bin` by adding an `export.bin.xml` patch file to your mod.

## Select InfoTip

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="500934" Add="">
      <!-- .. -->
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Add="@500934">
      <!-- .. -->
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp Type="add" Path="//InfoTipData[Guid='500934']">
      <!-- .. -->
    </ModOp>
    ```

## Structure

The structure of the InfoTips is different depending on the game version.

Anno 117 uses attributes to make InfoElements and VisibilityElements much more compact and readable.

=== ":material-pillar: 117"
    ```xml
    <InfoElement Type="Container">
      <VisibilityElement Type="Group">
        <VisibilityElement Type="Condition">
          <CompareOperator>Greater</CompareOperator>
          <!-- .. -->
        </VisibilityElement>
      </VisibilityElement>
      <!-- .. -->
    </InfoElement>
    ```

    !!! info "The counts `InfoTipCount`, `TemplateCount` and `ChildCount` are hidden in XML."

    Note: These improvements are not part of [FileDBReader](https://github.com/anno-mods/FileDBReader), except the type and operator names. Keep that in mind when comparing **FileDBReader** and **xmltest** output.

    !!! info "Reminder: Texts use OasisIDs now (`TextId`) instead of GUIDs (`TextGUID`)"

=== ":material-factory: 1800"
    ```xml
    <InfoElement>
      <ElementType>23</ElementType>
      <ChildCount>1</ChildCount>
      <VisibilityElement>
        <ElementType>
          <ElementType>2</ElementType>
        <ElementType>
        <VisibilityElement>
          <ElementType>
            <ElementType>1</ElementType>
          <ElementType>
          <ChildCount>1</ChildCount>
          <CompareOperator>
            <CompareOperator>5</CompareOperator>
          </CompareOperator>
          <!-- .. -->
        </VisibilityElement>
      </VisibilityElement>
      <!-- .. -->
    </InfoElement>
    ```

    !!! info "The counts `InfoTipCount`, `TemplateCount` and `ChildCount` will be adjusted automatically, you don't need to change anything."

    The format is the same as used in the [FileDBReader](https://github.com/anno-mods/FileDBReader/blob/master/FileFormats/infotip.xml).