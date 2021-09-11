<?php 
// This example is for keep old values in array of checkboxes with Laravel blade.

//1. Old checkbox when creanting a new register

<div class="row">
<div class="col-md-8">
    <div class="form-group">
       <label>Seleccionar una Calidad (Solo Una):<label style="color:#A83834">*</label></label>
       
       @foreach ($productQualities as $prQualitie)
      <div class="valid-feedback d-block" style="font-size: 15px !important;">
       <input onClick="ckChange(this)" type="checkbox" value="{{ $prQualitie->id }}" 
          name="checkboxQualities[]"
            @if(is_array(old('checkboxQualities')) && in_array("$prQualitie->id", old('checkboxQualities')))
                    checked
            @endif>
          <label>
           Calidad: {{ $prQualitie->name }}
                 
          </label>
        </div>   
       @endforeach
    </div>
    <div class="invalid-feedback d-block">
           @foreach ($errors->get('checkboxQualities') as $error)
                {{ $error }}
            @endforeach
        </div>
</div>


// 2. Nex example is for edit the checkboxe when you will update a register which has an array
// of checkboxes

<div class="row">
<div class="col-md-8">
    <div class="form-group">
       <label>Seleccionar calidades el Producto:<label style="color:#A83834">*</label></label>
       
       @foreach ($qualities as $quality)
      <div class="valid-feedback d-block" style="font-size: 15px !important;">
       <input onClick="ckChange(this)" type="checkbox" value="{{ $quality->id }}" 
          name="checkboxQualities[]"
            @if(is_array(old('checkboxQualities')) && in_array("$quality->id", old('checkboxQualities')))
                    checked
            @elseif(is_array($product_qualities) && in_array("$quality->id", $product_qualities))
              checked
            @endif>
          <label>
           Calidad: {{ $quality->name }}
                 
          </label>
        </div>   
       @endforeach
    </div>
    <div class="invalid-feedback d-block">
           @foreach ($errors->get('checkboxQualities') as $error)
                {{ $error }}
            @endforeach
        </div>
</div>
</div>

//3. If you want disable all the checkboxes  but not the checkbox of the product when edit, just add and else put disabled.
 name="checkboxQualities[]"
            @if(is_array(old('checkboxQualities')) && in_array("$quality->id", old('checkboxQualities')))
                    checked
            @elseif(is_array($product_qualities) && in_array("$quality->id", $product_qualities))
              checked
            @else
                disabled
            @endif>
//this will disable the rest of checkbox except the checkbox that belogns to the product.. in this case his quality.
                
                
//4. The next js function let disable the rest of checkboxes when one is checked
                
 @section('js')
  <script type="text/javascript">
        //Funcion para desactivar el resto de checkbox cuando un checkbox es marcado.
        
        function ckChange(el) {
            var ckName = document.getElementsByName(el.name);
            for (var i = 0, c; c = ckName[i]; i++) {
                c.disabled = !(!el.checked || c === el);
            }
        }

  </script>
@endsection
?>
