# Basic ModOp Types

## Replace

{{all}}

Replaces the selection.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Replace="Standard/Name">
      <Name>new name</Name>
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Replace="@123/Standard/Name">
      <Name>new name</Name>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="replace" Path="/Values/Standard">
      <Name>new name</Name>
    </ModOp>
    ```
=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
    -     <Name>old name</Name>
    +     <Name>new name</Name>
        </Standard>
      </Values>
    </Asset>
    ```

??? tip "Only replace if the value is higher."
    A common pattern is to replace a value only if it is higher than the existing one.

    This can be achieved with [Conditions](./control.md#condition).

    === ":material-pillar: 117"
        ```xml hl_lines="2"
        <ModOp GUID="2001241" Replace="BuildingBaseTiles"
               Condition="!BuildingBaseTiles[HarbourConstructionAreaRadius &gt;= 40]">
          <HarbourConstructionAreaRadius>40</HarbourConstructionAreaRadius>
        </ModOp>
        ```

        **Note on Brackets `<>`**

        Using brackets `<>` in attributes is not valid XML. Use `&lt;` for `<` and `&gt;` for `>` instead.

        **Note on Negative Condition**

        The condition is negative with `!`. You could do a positive condition with the same path as `Path`:

        ```
        Condition="BuildingBaseTiles[HarbourConstructionAreaRadius &lt; 40]"
        ```

        The difference is: if there is any error in the Condition path itself - be it a typo or another mod removed that element - you don't get any warnings. The condition will just say: not available, hence don't execute.

        On the other hand, the negative check with `!` skips if an conditions exists exactly how you expect it.
        Meaning it will execute if there is something unexpected, leading to warnings in the end.
    === ":material-animation-outline: 117 & 1800"
        ```xml hl_lines="2"
        <ModOp GUID="2001241" Type="replace" Path="/Values/BuildingBaseTiles"
               Condition="!/Values/BuildingBaseTiles[HarbourConstructionAreaRadius &gt;= 40]">
          <HarbourConstructionAreaRadius>40</HarbourConstructionAreaRadius>
        </ModOp>
        ```

        **Note on Brackets `<>`**

        Using brackets `<>` in attributes is not valid XML. Use `&lt;` for `<` and `&gt;` for `>` instead.

        **Note on Negative Condition**

        The condition is negative with `!`. You could do a positive condition with the same path as `Path`:

        ```
        Condition="/Values/BuildingBaseTiles[HarbourConstructionAreaRadius &lt; 40]"
        ```

        The difference is: if there is any error in the Condition path itself - be it a typo or another mod removed that element - you don't get any warnings. The condition will just say: not available, hence don't execute.

        On the other hand, the negative check with `!` skips if an conditions exists exactly how you expect it.
        Meaning it will execute if there is something unexpected, leading to warnings in the end.

Replace is not limited to single elements.
Elements containing elements can be replaced as well.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Replace="Standard">
      <Standard>
        <GUID>456</GUID>
        <Description>description</Description>
      </Standard>
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Replace="@123/Standard">
      <Standard>
        <GUID>456</GUID>
        <Description>description</Description>
      </Standard>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="replace" Path="/Values/Standard">
      <Standard>
        <GUID>456</GUID>
        <Description>description</Description>
      </Standard>
    </ModOp>
    ```

=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
    -     <GUID>123</GUID>
    -     <Name>name</Name>
    +     <GUID>456</GUID>
    +     <Description>description</Description>
        </Standard>
      </Values>
    </Asset>
    ```

??? info "An empty `Replace` is the same as `Remove`."
    === ":material-at: Replace"
        ```xml
        <ModOp Replace="@123/Cost" />
        ```

        Applies to all versions.

    === ":material-at: Remove"
        ```xml
        <ModOp Remove="@123/Cost" />
        ```

        Applies to all versions.

    === ":material-xml: Result"
        ```diff
        <Asset>
          <Values>
            <Standard>
              <GUID>123</GUID>
            </Standard>
        -   <Cost />
          </Values>
        </Asset>
        ```

??? warning "Avoid replacing larger structures."
    Replace removes all content in the selection and replaces it.

    That often leads to unforeseen issues after game updates or in combination with other mods.

## Add

{{all}}

Adds the content at the end inside of the selection.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Add="">
      <Maintenance />
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Add="@123">
      <Maintenance />
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="add" Path="/Values">
      <Maintenance />
    </ModOp>
    ```
=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
    +   <Maintenance />
      </Values>
    </Asset>
    ```

??? tip "`Add` does not check if an element already exists."
    Use [Conditions](./control.md#condition) to skip an `Add` if the node already exists.

    Use [Merge](#merge) to update if the node already exists, or add otherwise.

## Merge

{{all}}

Adds the content, or replaces it if it already exists.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Merge="Standard">
      <Standard>
        <Name>new name</Name>
        <Description>desc</Description>
      </Standard>
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Merge="@123/Standard">
      <Standard>
        <Name>new name</Name>
        <Description>desc</Description>
      </Standard>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="merge" Path="/Values/Standard">
      <Standard>
        <Name>new name</Name>
        <Description>desc</Description>
      </Standard>
    </ModOp>
    ```
=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
    -     <Name>old name</Name>
    +     <Name>new name</Name>
    +     <Description>desc</Description>
        </Standard>
      </Values>
    </Asset>
    ```

??? info "`Merge` does not remove nodes."

    === ":material-pillar: 117"
        ```xml
        <ModOp GUID="123" Merge="">
          <Building />
        </ModOp>
        ```
    === ":material-animation-outline: 117 & 1800"
        ```xml
        <ModOp GUID="123" Type="merge" Path="/Values">
          <Building />
        </ModOp>
        ```
    === ":material-xml: Result"
        ```diff hl_lines="5-7"
        <Values>
          <Standard>
            <GUID>123</GUID>
          </Standard>
          <Building>
            <AllowChangeVariation>1</AllowChangeVariation>
          </Building>
        </Values>
        ```

    Use `replace` or `remove` instead if you want to remove content.

??? info "`Merge` is order independent."

    The order of nodes do not matter for successful merges.

    === ":material-pillar: 117"
        ```xml
        <ModOp GUID="123" Merge="Building">
          <AllowChangeDirection>1</AllowChangeDirection>
          <AllowChangeVariation>1</AllowChangeVariation>
        </ModOp>
        ```
    === ":material-animation-outline: 117 & 1800"
        ```xml
        <ModOp GUID="123" Type="merge" Path="/Values/Building">
          <AllowChangeDirection>1</AllowChangeDirection>
          <AllowChangeVariation>1</AllowChangeVariation>
        </ModOp>
        ```
    === ":material-xml: Result"
        ```diff
          <Values>
            <Standard>
              <GUID>123</GUID>
            </Standard>
            <Building>
        -     <AllowChangeVariation>0</AllowChangeVariation>
        -     <AllowChangeDirection>0</AllowChangeDirection>
        +     <AllowChangeVariation>1</AllowChangeVariation>
        +     <AllowChangeDirection>1</AllowChangeDirection>
            </Building>
          </Values>
        ```

??? tip "Use `ModItem` {{a117}} or `Condition` {{all}} to merge list items."
    List items are merged by order of occurrence. It is better to replace a list completely or define conditions for each item individually.

    Read more about individual item conditions in [ModItem Merge](./lists.md#moditem-merge).

??? tip "Top-level content element can be skipped in most situations."
    The following is shorter by skipping `Standard` and has to the same result.

    === ":material-at: 117"
        ```xml
        <ModOp Merge="@123/Standard">
          <Name>new name</Name>
          <Description>desc</Description>
        </ModOp>
        ```

    === ":material-animation-outline: 117 & 1800"
        ```xml
        <ModOp GUID="123" Type="merge" Path="/Values/Standard">
          <Name>new name</Name>
          <Description>desc</Description>
        </ModOp>
        ```

    === ":material-xml: Result"
        ```diff
        <Asset>
          <Values>
            <Standard>
              <GUID>123</GUID>
        -     <Name>old name</Name>
        +     <Name>new name</Name>
        +     <Description>desc</Description>
            </Standard>
          </Values>
        </Asset>
        ```

    Only when the selected element has a child with the same name, you must use the long way.
    In doubt, always use the long way.

    === ":material-at: 117"
        ```xml
        <ModOp Merge="@123/Text">
          <Text>
            <Text>new text</Text>
          </Text>
        </ModOp>
        ```
    === ":material-animation-outline: 117 & 1800"
        ```xml
        <ModOp GUID="123" Type="Merge" Path="/Values/Text">
          <Text>
            <Text>new text</Text>
          </Text>
        </ModOp>
        ```
    === ":material-file-code-outline: Input"
        ```xml hl_lines="1 3"
          <Text>
            <LineId>123</LineId>
            <Text>old text</Text>
          </Text>
        ```
    === ":material-xml: Result"
        ```diff
          <Text>
            <LineId>123</LineId>
        -   <Text>old text</Text>
        +   <Text>new text</Text>
          </Text>
        ```

## Append (AddNextSibling)

{{all}}

Adds the content after the selection.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Append="Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Append="@123/Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="addNextSibling" Path="/Values/Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
    +   <Maintenance />
        <Cost />
      </Values>
    </Asset>
    ```

A common situation is when you want to insert an item at a specific position.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123"
           Append="ConstructionCategory/BuildingList/Item[Building='1000178']">
      <Item>
        <Building>123</Building>
      </Item>
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Append="@123/ConstructionCategory/BuildingList/Item[Building='1000178']">
      <Item>
        <Building>123</Building>
      </Item>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="addNextSibling"
           Path="/Values/ConstructionCategory/BuildingList/Item[Building='1000178']">
      <Item>
        <Building>123</Building>
      </Item>
    </ModOp>
    ```
=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
        <ConstructionCategory>
          <BuildingList>
            <Item>
              <Building>1000178</Building>
            </Item>
    +       <Item>
    +         <Building>123</Building>
    +       </Item>
            <Item>
              <Building>1010372</Building>
            </Item>
          </BuildingList>
        </ConstructionCategory>
      </Values>
    </Asset>
    ```

??? tip "Use `Merge` to append list items. {{a117}}"
    === ":material-pillar: 117"
        ```xml
        <ModOp GUID="123" Merge="ConstructionCategory/BuildingList">
          <ModItem Attribute="Building" Append="Building='1000178'">
            <Building>123</Building>
          </ModItem>
        </ModOp>
        ```
    === ":material-xml: Result"
        ```diff
          <Values>
            <Standard>
              <GUID>123</GUID>
            </Standard>
            <ConstructionCategory>
              <BuildingList>
                <Item>
                  <Building>1000178</Building>
                </Item>
        +       <Item>
        +         <Building>123</Building>
        +       </Item>
                <Item>
                  <Building>1010372</Building>
                </Item>
              </BuildingList>
            </ConstructionCategory>
          </Values>
        ```

    See also [Item Lists](./lists.md) for more details.

??? tip "Use `Asset` to add full assets. {{a117r}}"
    Directly drop assets into your file without `ModOp`.
    The order of `BaseAssetGUID` is automatically handled.

    === ":material-pillar: 117"
        ```xml
        <ModOps>
          <Asset>
            <BaseAssetGUID>100780</BaseAssetGUID>
            <Values> <!-- .. --> </Values>
          </Asset>
        </ModOps>
        ```
    === ":material-animation-outline: 117 & 1800 ⚠️"
        ```xml
        <ModOps>
          <ModOp Type="addNextSibling" GUID="100780">
            <Asset>
              <BaseAssetGUID>100780</BaseAssetGUID>
              <Values> <!-- .. --> </Values>
            </Asset>
          </ModOp>
        </ModOps>
        ```

## Prepend (AddPrevSibling)

{{all}}

Prepend works the same as `Append` except that it adds the content before the selection.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Prepend="Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Prepend="@123/Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="addPrevSibling" Path="/Values/Standard">
      <Maintenance />
    </ModOp>
    ```
=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
    +   <Maintenance />
        <Standard>
          <GUID>123</GUID>
        </Standard>
        <Cost />
      </Values>
    </Asset>
    ```

??? tip "Use `Merge` to prepend list items. {{a117}}"
    === ":material-pillar: 117"
        ```xml
        <ModOp GUID="123" Merge="ConstructionCategory/BuildingList">
          <ModItem Attribute="Building" Prepend="Building='1010372'">
            <Building>123</Building>
          </ModItem>
        </ModOp>
        ```
    === ":material-xml: Result"
        ```diff
          <Values>
            <Standard>
              <GUID>123</GUID>
            </Standard>
            <ConstructionCategory>
              <BuildingList>
                <Item>
                  <Building>1000178</Building>
                </Item>
        +       <Item>
        +         <Building>123</Building>
        +       </Item>
                <Item>
                  <Building>1010372</Building>
                </Item>
              </BuildingList>
            </ConstructionCategory>
          </Values>
        ```

    See also [Item Lists](./lists.md) for more details.


## Asset

{{a117r}}

The fastest way to add assets is to directly drop them into your file without using a `ModOp` at all.
The order of `BaseAssetGUID` is automatically handled.

=== ":material-pillar: 117"
    ```xml
    <ModOps>
      <Asset>
        <Template>Text</Template>
        <Values> <!-- .. --> </Values>
      </Asset>
      <Asset>
        <BaseAssetGUID>100780</BaseAssetGUID>
        <Values> <!-- .. --> </Values>
      </Asset>
    </ModOps>
    ```
=== ":material-animation-outline: 117 & 1800 ⚠️"
    ```xml
    <ModOps>
      <ModOp Type="addNextSibling" GUID="100780">
        <Asset>
          <Template>Text</Template>
          <Values> <!-- .. --> </Values>
        </Asset>
        <Asset>
          <!-- manually ensure that this comes after 100780 -->
          <BaseAssetGUID>100780</BaseAssetGUID>
          <Values> <!-- .. --> </Values>
        </Asset>
      </ModOp>
    </ModOps>
    ```

## Remove

{{all}}

Removes the selected elements.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="123" Remove="Cost" />
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Remove="@123/Cost" />
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp GUID="123" Type="remove" Path="/Values/Cost" />
    ```
=== ":material-xml: Result"
    ```diff
    <Asset>
      <Values>
        <Standard>
          <GUID>123</GUID>
        </Standard>
    -   <Cost />
      </Values>
    </Asset>
    ```

??? info "Removing an not existing element results in a log warning."
    As with all ModOps, there will be a warning if the selected element path does not exist.