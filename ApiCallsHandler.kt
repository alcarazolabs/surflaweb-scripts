package com.example.latienda.core
import android.util.Log
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import okhttp3.ResponseBody
import org.json.JSONObject
import retrofit2.HttpException
import java.io.IOException
import java.net.SocketTimeoutException

//Video: https://youtu.be/rWbv2zU4Zqw


object ApiCallsHandler {

    private const val MESSAGE_KEY = "message"
    private const val ERROR_KEY = "error"

    enum class ErrorType {
        NETWORK, // IO
        TIMEOUT, // Socket
        UNKNOWN //Anything else
    }

    public fun getErrorMessage(responseBody: ResponseBody?): String {
        return try {
            val jsonObject = JSONObject(responseBody!!.string())
            when {
                jsonObject.has(MESSAGE_KEY) -> jsonObject.getString(MESSAGE_KEY)
                jsonObject.has(ERROR_KEY) -> jsonObject.getString(ERROR_KEY)
                else -> "Something wrong happened"
            }
        } catch (e: Exception) {
            "Something wrong happened"
        }
    }

    interface RemoteErrorEmitter {
        fun onError(msg: String)
        fun onError(errorType: ErrorType)
    }

    public suspend inline fun <T> safeApiCall(
        emitter: RemoteErrorEmitter,
        crossinline responseFunction: suspend () -> T,
    ): T? {
        return try {
            val response = withContext(Dispatchers.IO) { responseFunction.invoke() }
            response
        } catch (e: Exception) {
            withContext(Dispatchers.Main) {
                e.printStackTrace()
                Log.e("ApiCalls", "Call error: ${e.localizedMessage}", e.cause)
                when (e) {
                    is HttpException -> {
                        val body = e.response()?.errorBody()
                        emitter.onError(getErrorMessage(body))
                    }
                    is SocketTimeoutException -> emitter.onError(ErrorType.TIMEOUT)
                    is IOException -> emitter.onError(ErrorType.NETWORK)
                    else -> emitter.onError(ErrorType.UNKNOWN)
                }
            }
            null
        }
    }


}